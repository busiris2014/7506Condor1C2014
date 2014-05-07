#include "HashTable.h"

#include "../common/FileBlockManager.h"
#include"../common/ByteString.h"
#include "../common/Constantes.h"
#include <sstream>

HashTable::HashTable(FileBlockManager* file, FreeBlockManager* freeBlocks){
	this->file = file;
	this->freeBlocks = freeBlocks;

	if(!this->file->fileOpen()){
		this->blockList.push_back(0);
		this->referenceList.push_back(new Reference(1));

		this->file->fileNew();
		this->file->fileOpen();
		this->file->insertBlock();
		this->saveHashTable();
	}
	else
	{
		this->loadHashTable();
	}
}

HashTable::~HashTable() {

	vector<Reference*>::const_iterator iter;
	for(iter = this->referenceList.begin(); iter != this->referenceList.end(); iter++)
		delete *iter;
}

int HashTable::addNewBlock()
{
	int newBlock;
	if (this->freeBlocks->isNotEmpty())
		newBlock = this->freeBlocks->getFirstFreeBlock();
	else
	{
		file->insertBlock();
		newBlock = file->getBlockCount() - 1;
	}

	this->blockList.push_back(newBlock);
	return newBlock;
}

void HashTable::loadHashTable()
{

	if(!this->referenceList.empty())
	{
		//limpia la lista de referencias
		vector<Reference*>::const_iterator iter;
		for(iter = this->referenceList.begin(); iter != this->referenceList.end(); iter++)
			delete *iter;
		this->referenceList.clear();

		//limpia la lista de blockes
		this->blockList.clear();
	}

	//carga en memoria toda la hash table
	bool haveNext = true;
	int nextBlock = 0;
	int sizeReference = Reference::getSize();
	while (haveNext)
	{
		this->blockList.push_back(nextBlock);
		ByteString blockTable(this->file->readBlock(nextBlock));
		int referenceCount = blockTable.readAsInt(0);
		nextBlock = blockTable.readAsInt(sizeof(int));

		ByteString reference_i;
		for (int i = 0; i < referenceCount; i++)
		{
			int offset = (i * sizeReference) + (2 * sizeof(int));
			ByteString reference_i = blockTable.read(offset,sizeReference);

			Reference* newReference = new Reference();
			newReference->Hidratate(reference_i);
			this->referenceList.push_back(newReference);
		}

		haveNext = (nextBlock > -1);
	}
}

void HashTable::saveHashTable()
{
	ByteString byteString;
	int referencePerBlock = (file->getBlockSize() - (2*sizeof(int))) / Reference::getSize();

	bool lastBlock = false;
	unsigned int indexListReference = 0;
	unsigned int indexListBlock = 0;
	unsigned int actualBlock;
	int nextBlock;
	while (!lastBlock)
	{
		byteString.clean();

		//Genera la cadena de las referencias a persistir
		int referenceInBlock = 0;
		while (this->referenceList.size() > indexListReference && referenceInBlock < referencePerBlock)
		{
			byteString.insertLast(this->referenceList[indexListReference]->Serialize());

			referenceInBlock++;
			indexListReference++;
		}

		//Obtiene el numero de bloque donde se persistira
		if (this->blockList.size() >= indexListBlock + 1)
			actualBlock = this->blockList[indexListBlock];
		else
			actualBlock = this->addNewBlock();

		//Si no es el ultimo bloque, obtiene el valor del siguiente bloque, si es el ultimo lo setea en -1
		if (referenceInBlock == referencePerBlock && this->referenceList.size() > indexListReference + 1)
		{
			if (this->blockList.size() >= indexListBlock + 2)
				nextBlock = this->blockList[indexListBlock + 1];
			else
				nextBlock = this->addNewBlock();
		}
		else
		{
			lastBlock = true;
			nextBlock = -1;
		}

		//Inserta al principio la cadena con la metadata (Cantidad de Referencias y Siguiente bloque)
		byteString.insert(&nextBlock, sizeof(int), 0);
		byteString.insert(&referenceInBlock, sizeof(int), 0);
		byteString.fillWithEmpty(file->getBlockSize());

		//Y listo... magicamente persiste!
		file->saveBlock(actualBlock, byteString);

		if (!lastBlock)
			indexListBlock++;
	}

	//Si hay bloques que no se usaron, se liberan
	if (indexListBlock + 1 < this->blockList.size())
	{
		for (unsigned int i = indexListBlock + 1; i < this->blockList.size(); i++)
		{
			this->freeBlocks->insertFreeBlock(this->blockList[i]);
		}

		//Actualiza el vector
		this->blockList.erase(this->blockList.begin() + indexListBlock, this->blockList.end());
	}
}

bool HashTable::truncateTable()
{
	bool success = true;
	unsigned int newSizeTable = this->referenceList.size() / 2;

	//Libera las referencias de la segunda mitad de la tabla
	for (unsigned int i = newSizeTable; i < this->referenceList.size(); i++)
	{
		delete this->referenceList[i];
	}

	//Actualiza el vector
	this->referenceList.erase(this->referenceList.begin() + newSizeTable, this->referenceList.end());

	//Actualiza los cambios en disco
	this->saveHashTable();

	return success;
}

void HashTable::duplicateTable(int referenceNumber, int newBucketNumber){
	int oldSize = this->referenceList.size();
	Reference* newReference;

	for(int i=0; i<oldSize; i++)
	{
		newReference = new Reference(this->referenceList[i]->getPosition());
		this->referenceList.push_back(newReference);
	}

	//Actualiza la referencia del overflow
	this->setBlock(referenceNumber, newBucketNumber);

	//Se guardan los cambios en disco
	this->saveHashTable();
}


bool HashTable::needTruncate(){
	bool trunc = true;
	int pos = 0;
	int halfSizeTable = referenceList.size()/2;

	if(this->referenceList.size() == 1)
	{
		trunc = false;
	}
	else
	{
		while(trunc && pos < (halfSizeTable))
		{
			if(referenceList[pos]->getPosition() == referenceList[pos + halfSizeTable]->getPosition())
				pos++;
			else
				trunc = false;
		}

		if(trunc)
		{
			trunc = this->truncateTable();
		}
	}

	return trunc;
}

void HashTable::updateReferences(int initialReference, int displacement, int newBlockNumber)
{
	// Se cambia a donde apunta el bloque que dio overflow
	this->setBlock(initialReference, newBlockNumber);

	int forward = initialReference + displacement;
	int behind = initialReference - displacement;

	while(forward < this->getSize())
	{
		this->setBlock(forward, newBlockNumber);
		forward = forward + displacement;
	}

	while(behind >= 0)
	{
		this->setBlock(behind, newBlockNumber);
		behind = behind - displacement;
	}

	// Se graba los cambios
	this->saveHashTable();
}

int HashTable::findDeletableBlock(int initialReference, int displacement){
	bool ok = true;
	int referenceSize = this->referenceList.size();

	int forward = initialReference + displacement;
	if(forward >= referenceSize)
		forward = forward - referenceSize;

	int behind = initialReference - displacement;
	if(behind < 0)
		behind = referenceSize + behind;

	int newBlock = this->getBlock(forward);

	ok = (newBlock == this->getBlock(behind));

	if(!ok)
		newBlock = -1;

	return newBlock;
}

int HashTable::getSize() const
{
    return this->referenceList.size();
}

int HashTable::getBlock(int indexReference)
{
	return this->referenceList[indexReference]->getPosition();
}

void HashTable::setBlock(int indexReference, int position)
{
	this->referenceList[indexReference]->setPosition(position);
}

set<int> HashTable::getBucketBlocks()
{
	set<int> bucketBlocks;
	vector<Reference*>::iterator refIt;
	for(refIt = this->referenceList.begin(); refIt!=this->referenceList.end(); ++refIt)
	{
		int bucketBlock = (*refIt)->getPosition();
		if (bucketBlocks.count(bucketBlock) == 0)
			bucketBlocks.insert(bucketBlock);
	}
	return bucketBlocks;
}

string HashTable::TabletoString()
{
	string str;
	for(unsigned int i = 0; i < this->referenceList.size(); i++)
	{
		str += Utility::intToString(i) + "\t | \t";
		str += Utility::intToString(this->referenceList[i]->getPosition());
		str += "\n";
	}
	return str;
}

string HashTable::BlockstoString()
{
	string text;
	text += "Bloques de la Tabla de Dispersion (Cant.Referencias|Prox.Bloque|Referencias)\n";

	int nextBlock = 0;
	int actualBlock = 0;
	string textReferences;

	while (nextBlock>-1)
	{
		textReferences = "";
		actualBlock = nextBlock;

		ByteString blockTable(this->file->readBlock(nextBlock));

		int referenceCount = blockTable.readAsInt(0);
		nextBlock = blockTable.readAsInt(sizeof(int));

		ByteString reference_i;
		for (int i = 0; i < referenceCount; i++)
		{
			int offset = (i *  Reference::getSize()) + (2 * sizeof(int));
			ByteString reference_i = blockTable.read(offset, Reference::getSize());

			Reference* newReference = new Reference();
			newReference->Hidratate(reference_i);
			textReferences = textReferences + newReference->toString() + ",";
			delete newReference;
		}

		textReferences = textReferences.substr(0, textReferences.length()-1);
		text += "Bloque " + Utility::intToString(actualBlock) + ": " + Utility::intToString(referenceCount);
		text += "|" +  Utility::intToString(nextBlock) + "|" + textReferences + "\n";
	}

	return text;
}
