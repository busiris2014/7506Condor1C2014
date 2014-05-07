#include "ListofID.h"
#include "../Utility/Utility.h"

ListofID::ListofID(FileBlockManager* fileBlockManager, int idBlockBegin)//:listOfBlock(), listOfID(new ComparerRecord())
{
	this->fileBlockManager = fileBlockManager;
	this->freeBlockManager = NULL;
	if(fileBlockManager!=NULL && idBlockBegin>=0)
		this->readListinFile(idBlockBegin);
}

ListofID::ListofID(FileBlockManager* fileBlockManager, FreeBlockManager* freeBlockManager, int idBlockBegin)//:listOfBlock()//, listOfID(new ComparerRecord())
{
	this->fileBlockManager = fileBlockManager;
	this->freeBlockManager = freeBlockManager;
	if(fileBlockManager!=NULL && idBlockBegin>=0)
		this->readListinFile(idBlockBegin);
}

ListofID::ListofID(FileBlockManager* fileBlockManager, FreeBlockManager* freeBlockManager, int idBlockBegin,bool compressed)
{
	this->fileBlockManager = fileBlockManager;
	this->freeBlockManager = freeBlockManager;
	if(fileBlockManager!=NULL && idBlockBegin>=0)
		this->readListinFile(idBlockBegin);

	this->decompress();
}

ListofID::~ListofID()
{
	//this->listOfID.clear();
	//this->listOfBlock.clear();
}


bool ListofID::addID(int ID)
{
	bool found = false;
	list<int>::iterator it;
	for(it = listOfID.begin();it != listOfID.end(); ++it)
	{
			if((*it) == ID)
			{
				found = true;
				break;
			}
	}
	if(!found)
	{
		listOfID.push_back(ID);
	}
	return !found;
}

bool ListofID::removeID(int ID)
{
	bool found = false;
	list<int>::iterator it;
	for(it = listOfID.begin();it != listOfID.end(); ++it)
	{
			if((*it) == ID)
			{
				found = true;
				//listOfOcurrences.remove(*it);
			}
	}
	return found;
}

list<int>* ListofID::cloneListID()
{
	list<int>* listOfInt = new list<int>;
	list<int>::iterator it;

	for(it = listOfID.begin();it!=listOfID.end();++it)
	{
		int ID = *it;
		listOfInt->push_back(ID);
	}
	return listOfInt;
}

list<int> ListofID::getListID()
{
//	List<int> list;
//	ListIterator<int> it = listOfID.getIterator();
//
//	while(it.hasNext())
//	{
//		int ID = it.next();
//		list.insertData(ID);
////		if(ID == 0) // FIXME: esto esta mal
////			break;
//	}
	return this->listOfID;
}

string ListofID::toString()
{
	string text("\t\t");
	text += "Bloques: ";

	list<int>::iterator blockIt;
	for(blockIt = listOfBlock.begin();blockIt!=listOfBlock.end();++blockIt)
	{
		text += Utility::intToString(*blockIt);
		text += ", ";
	}

	text += " - Ids: ";

	list<int>::iterator idIt;
	for(idIt = listOfID.begin(); idIt != listOfID.end();++idIt)
	{
		text += Utility::intToString(*idIt);
		text += ", ";
	}

	text += "\n";

	return text;
}

int ListofID::getNewBlock()
{
	int newBlock;
	if (this->freeBlockManager != NULL && this->freeBlockManager->isNotEmpty())
		newBlock = this->freeBlockManager->getFirstFreeBlock();
	else
	{
		fileBlockManager->insertBlock();
		newBlock = fileBlockManager->getBlockCount() - 1;
	}

	return newBlock;
}

//Funcion ingresa un nuevo bloque como bloque libre
int ListofID::setFreeBlock(int block)
{
	if (this->freeBlockManager != NULL)
		this->freeBlockManager->insertFreeBlock(block);

	return block;
}

int ListofID::writeListinFile()
{
	int idFreeBlock = -1;

		//Formato de escritura: CantIds,ProximoBloque,Id1,Id2,...,Idn,0...
		ByteString byteString;
		int idPerBlock = (fileBlockManager->getBlockSize() - (2*sizeof(int))) / sizeof(int);

		int idInBlock = 0;
		int actualBlock = 0;
		int nextBlock = 0;
		list<int>::iterator intIt;
		list<int>::iterator idBlockIt;

		intIt = listOfID.begin();
		idBlockIt = listOfBlock.begin();
		if (intIt != listOfID.end())
		{
			if (idBlockIt != listOfBlock.end())
			{
				nextBlock = *idBlockIt;
			}
			else
				nextBlock = this->getNewBlock();

			while(intIt != (listOfID.end()))
			{
				actualBlock = nextBlock;
				byteString.clean();
				idInBlock = 0;

				while((intIt != (listOfID.end())) && (idPerBlock > idInBlock))
				{
					int id = *intIt;
					byteString.insertLast(&id, (int)sizeof(int));
					idInBlock++;
					intIt++;
				}

				//Obtiene el numero del proximo bloque donde grabar
				if (intIt != (listOfID.end()))
				{
					idBlockIt++;
					if (idBlockIt != listOfBlock.end())
					{
						nextBlock = *idBlockIt;
					}
					else
						nextBlock = this->getNewBlock();
				}
				else
					nextBlock = -1;

				//Inserta al principio la cadena con la metadata (Cantidad de Referencias y Siguiente bloque)
				byteString.insert(&nextBlock, sizeof(int), 0);
				byteString.insert(&idInBlock, sizeof(int), 0);
				byteString.fillWithEmpty(fileBlockManager->getBlockSize());

				fileBlockManager->saveBlock(actualBlock, byteString);
			}

			idFreeBlock = -1;
			idBlockIt++;
			while(idBlockIt != listOfBlock.end())
			{
				idFreeBlock = *idBlockIt;
				this->setFreeBlock(idFreeBlock);
				idBlockIt++;
			}
		}
		return idFreeBlock;
}

int ListofID::writeListinFile(bool compressed)
{
	this->compress();
	return this->writeListinFile();
}

void ListofID::readListinFile(int idBlockBegin)
{
	int idNextBlock = idBlockBegin;
	this->countBlock = 0;

	int sizeReference = sizeof(int);
	int idsCount = 0;
	while(idNextBlock >= 0)
	{
		//Agrega el bloque a la lista
		this->countBlock++;
		this->listOfBlock.push_back(idNextBlock);

		//Lee el bloque, y interpreta el registro
		ByteString stringOfByte = fileBlockManager->readBlock(idNextBlock);
		idsCount = stringOfByte.readAsInt(0);
		if (idsCount > 0)
		{
			idNextBlock = stringOfByte.readAsInt(sizeof(int));

			//Lee cada uno de los ids guardados en el bloque
			for (int i = 0; i < idsCount; i++)
			{
				int offset = (i * sizeReference) + (2 * sizeReference);
				int id = stringOfByte.readAsInt(offset);

				this->listOfID.push_back(id);
			}
		}
		else
			idNextBlock = -1;
	}
}

void sortArray(int *array,int size)
{
	int temp=0;
	for(int i=0;i<size-1;i++)
		for(int j=i+1;j<size;j++)
		{
			if (array[i] > array[j])
			{
				temp = array[i];
			    array[i] = array[j];
			    array[j] = temp;
			}
		}
}

void ListofID::compress()
{

	void *buffer=0;

	int idcount=this->listOfID.size();
	if(idcount>0){
		int* sortedArray=(int*) malloc(idcount*sizeof(int));


		list<int>::iterator intIt;
		intIt = this->listOfID.begin();
		int i=0;
		while (intIt != this->listOfID.end())
		{
					sortedArray[i]=*intIt;
					intIt++;
					i++;
		}
		/*
		ListIterator<int> iterator=this->listOfID.getIterator();
		int i=0;
		while(iterator.hasNext())
		{
			sortedArray[i]=iterator.next();
			i++;
		}
*/
//		sortArray(sortedArray,idcount);

		gammaCoder coder;
		coder.reset();
		int bitesaux=0;
		int lastid=0;
		for(int j=0;j<idcount;j++)
		{
			coder.encode(sortedArray[j]-lastid,bitesaux,&buffer);
			lastid=sortedArray[j];
		}

		this->listOfID.erase(this->listOfID.begin(),this->listOfID.end());

		this->addID(idcount);
		int newcount = floor(coder.nbytesallocked/(double)sizeof(int));
		int extrabytes=coder.nbytesallocked%sizeof(int);
		int intaux=0;
		i=0;
		for(i=0;i<newcount;i++)
		{
			memcpy(&intaux,((unsigned char*)buffer+i*sizeof(int)),sizeof(int));
			this->addID(intaux);
			intaux=0;
		}
		memcpy(&intaux,((unsigned char*)buffer+newcount*sizeof(int)),extrabytes);
		this->addID(intaux);
		free(sortedArray);
		free(buffer);
	}

}

void ListofID::decompress()
{

	int idcount=this->listOfID.size();
	if(idcount>1)
	{
		int* sortedArray=(int*) malloc(idcount*sizeof(int));
		list<int>::iterator intIt;
		intIt = this->listOfID.begin();
		int i=0;

		int resultidcount=0;
		if(intIt != this->listOfID.end())
		{
			resultidcount=*intIt;
			intIt++;
		}
		while (intIt != this->listOfID.end())
		{
			sortedArray[i]=*intIt;
			intIt++;
			i++;
		}

		this->listOfID.erase(this->listOfID.begin(),this->listOfID.end());
		gammaCoder coder;
		coder.reset();
		int lastid=0;
		unsigned int result=0;
		for(int j=0;j<resultidcount;j++)
		{
			coder.decode(sortedArray, result);
			this->addID(((int)result)+lastid);
			lastid+=result;
		}
		free(sortedArray);
	}
}


