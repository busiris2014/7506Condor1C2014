/*
 * ClassifBPlusTree.cpp
 *
 *  Created on: 21/04/2011
 *      Author: luciano
 */

#include "ClassifBPlusTree.h"

ClassifBPlusTree::ClassifBPlusTree(string filePath, int maxKeySize):BPlusTree()
{
	fileBlockNodeMapper = new FileBlockNodeMapper(filePath);
	this->firstLeaf = 0;
	this->keyTopSize = maxKeySize;

	this->fileBlockManager = new FileBlockManager(filePath + ".tree", TreeConstraits::getFullSizeNode());
	if(!this->fileBlockManager->fileOpen()){
		this->fileBlockManager->fileNew();
		this->fileBlockManager->fileOpen();
	}

	this->fileBlockNodeMapper->addBlockNodeRecord(0,0);

	this->root = hidratateNode(0);
	this->fileBlockManager->insertBlock();

	if (this->root) {
		this->NodesMount = fileBlockNodeMapper->getSizeList();
	} else {
		this->NodesMount = 0;
	}
	hidratateDataConfig();
}

ClassifBPlusTree::~ClassifBPlusTree()
{
	delete this->fileBlockNodeMapper;
}
void ClassifBPlusTree::persistNode(Node* n)
{
	int block = fileBlockNodeMapper->getBlock(n->number);
	if(block>-1)
	{
		fileBlockManager->saveBlock(block, n->Serialize());
	}
	else
	{
		fileBlockManager->insertBlock();
		block = fileBlockManager->getBlockCount() - 1;
		fileBlockNodeMapper->addBlockNodeRecord(n->number,block);
		fileBlockManager->saveBlock(block,n->Serialize());
	}
}
Node* ClassifBPlusTree::hidratateNode(int nodeNumber)
{
	int block = fileBlockNodeMapper->getBlock(nodeNumber);
	ByteString byteStr = this->fileBlockManager->readBlock(block);
	if (byteStr.isEmpty()) {
		return NULL;
	} else {
		int nivel = byteStr.readAsInt(0);
		if (nivel == 0) {
			LeafNode *nuevoNodoHoja = createLeafNode();
			nuevoNodoHoja->Hidratate(byteStr);
			nuevoNodoHoja->number = nodeNumber;
			return nuevoNodoHoja;
		} else {
			InnerNode *nuevoNodoInterior = createInnerNode(nivel);
			nuevoNodoInterior->Hidratate(byteStr);
			nuevoNodoInterior->number = nodeNumber;
			return nuevoNodoInterior;
		}
	}
}

bool ClassifBPlusTree::add(Record* record)
{
	bool toReturn;
	Key k(record->getKey()->getKey());
	pair<Record*, BPlusTreeIterator*> searchResult = BPlusTree::search(k);
	int idToAdd = record->getData()->readAsInt(0);
	if(searchResult.first != NULL) // la editorial o autor ya existe como clave en el padre
	{
		int listOffset = searchResult.first->getData()->readAsInt(0);
		toReturn = addToListofId(listOffset,idToAdd);
	}
	else // la editorial o el autor no existe, por ende tengo que dar de alta un record en el padre
	{
		this->fileBlockManager->insertBlock();
		int idBlock = fileBlockManager->getBlockCount() - 1;
		ByteString idBlockData;
		idBlockData.insertLast(&idBlock, sizeof(int));
		Record* parentRec = new Record(record->getKey(), &idBlockData);
		BPlusTree::add(parentRec);
		//delete parentRec;
		toReturn = addToListofId(idBlock,idToAdd);
	}
	return toReturn;
}


bool ClassifBPlusTree::removeKey(Key key,int idToRemove)
{
	bool toReturn;
	pair<Record*, BPlusTreeIterator*> searchResult = BPlusTree::search(key);
	if(searchResult.first != NULL)
	{
		int listOffset = searchResult.first->getData()->readAsInt(0);
		ListofID idList(this->fileBlockManager, listOffset);
		toReturn = idList.removeID(idToRemove);
		if(idList.getListID().size() == 0)
			toReturn = BPlusTree::removeKey(key);
		else
			idList.writeListinFile();
	}
	else
	{
		return false;
	}
	return toReturn;
}


list<int>* ClassifBPlusTree::searchAllIds(Key k){
	pair<Record*, BPlusTreeIterator*> searchResult = BPlusTree::search(k);
	list<int>* toReturn = new list<int>;

	if(searchResult.first != NULL){
		int listOffset = searchResult.first->getData()->readAsInt(0);
		ListofID idList(this->fileBlockManager, listOffset);
		list<int>::iterator it;
		list<int> listOfIds = idList.getListID();
		for(it=listOfIds.begin();it!=listOfIds.end();++it)
		{
			int current = *it;
			toReturn->push_back(current);
		}
	}
	return toReturn;
}

void ClassifBPlusTree::exportNode(ofstream& salida, Node* unNodo, int tabulacion,bool keytext, bool textContent) {

	if (unNodo->isLeaf()) {

		LeafNode *unNodoHoja = static_cast<LeafNode*> (unNodo);
		salida << endl;
		for(int i = 0 ; i < tabulacion ; i++)
			salida << "  ";

		salida << "Numero: " << unNodoHoja->number << "  Nivel: " << unNodoHoja->level << "  Cant.Elem: " << unNodoHoja->keyMount << "  Esp.Libre: " << TreeConstraits::getEfectiveSizeNode() - unNodoHoja->occupiedSpace << "  Hoja.Sig: " << unNodoHoja->nextLeaf << endl;

		for (int posicion = 0; posicion < unNodoHoja->keyMount; ++posicion) {

			for(int i = 0 ; i < tabulacion + 4 ; i++)
						salida << "  ";

			salida << "(";

			Key unaClave = unNodoHoja->keys[posicion];
			if(keytext)
			{
				salida << unaClave.toString();
			}
			else
			{
				ByteString clave(unaClave.toString());
				if(clave.getSize()==(2*sizeof(int)))
				{
					salida << Utility::toString(clave.readAsInt(0));
					salida << "-";
					salida << Utility::toString(clave.readAsInt(sizeof(int)));
				}
				else
					salida << Utility::toString(clave.readAsInt(0));
			}

			salida << ";";

			ByteString unDato = unNodoHoja->byteData[posicion];

			int idBlock = unDato.readAsInt(0);
			string datoString = Utility::intToString(idBlock);
			salida << datoString;

			salida << ")";

			salida << (unaClave.getSize() + unDato.getSize() + TreeConstraits::getControlSizeRecord()) << endl;

			for(int i = 0 ; i < tabulacion + 4 ; i++)
								salida << "  ";

			ListofID listOfID(this->fileBlockManager,idBlock);
			list<int> listOfInt = listOfID.getListID();
			list<int>::iterator it;

			int id;
			//int count = 0;
			ByteString bs;
			unsigned int i = 0;
			for(it=listOfInt.begin();it!=listOfInt.end();++it)
			{
				i++;
				id = *it;
				bs.insertLast(Utility::intToString(id));
				bs.insertLast(" ");
				if((i == listOfInt.size()) || (bs.toString().size() >80))
				{
					salida << bs.toString();
					salida << endl;
					if(i != listOfInt.size())
					{
						for(int i = 0 ; i < tabulacion + 4 ; i++)
											salida << "  ";
					}
					bs.clean();
				}
			}

		}

	} else {

		InnerNode *unNodoInterior = static_cast<InnerNode*> (unNodo);
		salida << endl << endl;
		for(int i=0; i<tabulacion ; i++)
			salida << "  ";

		salida << "Numero: " << unNodoInterior->number << "  Nivel: " << unNodoInterior->level << "  Cant.Elem: " << unNodoInterior->keyMount
			   << "  Esp.Libre: " << TreeConstraits::getEfectiveSizeNode() - unNodoInterior->occupiedSpace << endl;

		for (int posicion = 0; posicion <= unNodoInterior->keyMount; ++posicion) {

			if (posicion < unNodoInterior->keyMount) {
				Key unaClave = unNodoInterior->keys[posicion];

				for(int i=0; i<(tabulacion+1) ; i++)
					salida << "  ";
				salida << "(";
				if(keytext)
				{
					salida << unaClave.toString();
				}
				else
				{
					ByteString clave(unaClave.toString());
					salida << Utility::toString(clave.readAsInt(0));
				}
				salida << ")";
				salida << unaClave.getSize();
				salida << "  ";
				salida << unNodoInterior->getSons()[posicion] << "   ";
			}
		}
		for (int posicion = 0; posicion <= unNodoInterior->keyMount; ++posicion) {
			Node *hijo = hidratateNode(unNodoInterior->sons[posicion]);
			exportNode(salida, hijo, tabulacion + 2,keytext,textContent);
			if (hijo)
				freeNodeMemory(hijo);
		}
		for(int i=0; i<tabulacion ; i++)
			salida << "  ";
		salida << endl;
	}
}

bool ClassifBPlusTree::addToListofId(int listNumber, int idToAdd)
{
	ListofID listofID(this->fileBlockManager,listNumber);

	//Se agrega a la lista, si se agrega exitosamente (no estaba ya en la lista) se graba
	bool success = listofID.addID(idToAdd);
	if (success)
		listofID.writeListinFile();

	return success;
}

bool ClassifBPlusTree::addList(Record* record,list<unsigned int> listToAdd)
{
	Key k(record->getKey()->getKey());
	pair<Record*, BPlusTreeIterator*> searchResult = BPlusTree::search(k);
	//int idToAdd = record->getData()->readAsInt(0);
	if(searchResult.first != NULL) // la editorial o autor ya existe como clave en el padre
	{
		int listOffset = searchResult.first->getData()->readAsInt(0);
		addListToListofId(listOffset,listToAdd);

		delete searchResult.first;
	}
	else // la editorial o el autor no existe, por ende tengo que dar de alta un record en el padre
	{
		this->fileBlockManager->insertBlock();
		int idBlock = fileBlockManager->getBlockCount() - 1;
		ByteString* idBlockData = new ByteString();
		idBlockData->insertLast(&idBlock, sizeof(int));
		Record* parentRec = new Record(new Key(record->getKey()->getKey()), idBlockData);
		BPlusTree::add(parentRec);
		delete parentRec;
		addListToListofId(idBlock,listToAdd);
	}

	if(searchResult.second != NULL)
		delete searchResult.second;
	return true;
}

void ClassifBPlusTree::addListToListofId(int listNumber, list<unsigned int> listToAdd)
{
	ListofID listofID(this->fileBlockManager,listNumber);
	list<unsigned int>::iterator it;
	//Se agregan ids a la lista
	for(it=listToAdd.begin();it!=listToAdd.end();++it)
	{
		listofID.addID(*it);
	}
	listofID.writeListinFile();
}
