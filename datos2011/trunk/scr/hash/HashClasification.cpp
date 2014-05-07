#include "HashClasification.h"
#include "../common/Constantes.h"

HashClasification::HashClasification(string path): Hash(path)
{
}

HashClasification::~HashClasification()
{
}

list<int>* HashClasification::getToListofId(int listNumber)
{
	ListofID listofID(this->blockPersistor, this->freeBlocks, listNumber, true);
	list<int>* list = listofID.cloneListID();
	return list;
}

bool HashClasification::addToListofId(int listNumber, int newDataId)
{
	ListofID listofID(this->blockPersistor, this->freeBlocks, listNumber, true);

	//Se agrega a la lista, si se agrega exitosamente (no estaba ya en la lista) se graba
	bool success = listofID.addID(newDataId);
	if (success)
		listofID.writeListinFile(true);

	return success;
}

int HashClasification::removeFromListofId(int listNumber, int dataId)
{
	int countIds = 0;
	ListofID listofID(this->blockPersistor, this->freeBlocks, listNumber, true);

	//Se agrega a la lista, si se agrega exitosamente (no estaba ya en la lista) se graba
	bool success = listofID.removeID(dataId);
	if (success)
	{
		list<int> list = listofID.getListID();
		countIds = list.size();

		listofID.writeListinFile(true);
	}
	else
		countIds = -1;

	return countIds;
}

bool HashClasification::insertRecord (Record* newData)
{
	bool success = true;
	int blockPosition = this->hashKey(newData->getKey());
	int bucketNumber = this->hashTable->getBlock(blockPosition);
	Bucket* newBucket = this->readBucket(bucketNumber);

	//obtiene los datos del record: idLibro, idTermino
	int newListDataId = newData->getData()->readAsInt(sizeof(int)*0);
	int newId = newData->getData()->readAsInt(sizeof(int)*1);

	Record* record = newBucket->find(newData->getKey());
	if(record)
	{
		//Si se encontro el id en el record, se obtiene la lista que corresponde
		int listNumber = record->getData()->readAsInt(0);
		this->addToListofId(listNumber, newListDataId);
	}
	else
	{
		//obtiene el tamanio de nuevo registro, que es la clave mas un entero
		//sin obtener el nuevo bloque, recien si hay tamanio disponible pide un nuevo bloque
		int newListNumber = 0;
		//record = newData->clone();
		//record->setData(new ByteString(&newListNumber, sizeof(int)*0));

		record = newData->clone();
		ByteString* recordData = new ByteString();
		recordData->insertLast(&newListNumber, sizeof(int));
		recordData->insertLast(&newId, sizeof(int));
		record->setData(recordData);

		//Si hay capacidad, se inserta en el bucket y se graba, sino se reestructura la tabla.
		if(newBucket->getAvailableSpace() >= record->getSize())
		{
			//Crea un bloque nuevo, y crea el registro a insertar en el hash
			newListNumber = this->findNewBlock();
			//record->setData(new ByteString(&newListNumber, sizeof(int)));
			recordData = new ByteString();
			recordData->insertLast(&newListNumber, sizeof(int));
			recordData->insertLast(&newId, sizeof(int));
			record->setData(recordData);
			newBucket->insertRecord(record);

			//Se graba el bucket y la lista invertida
			this->blockPersistor->saveBlock(bucketNumber, newBucket->Serialize());
			this->addToListofId(newListNumber, newListDataId);
		}
		else
		{
			//Borra el registro
			delete record;

			this->resolveBucketOverflow(newBucket, bucketNumber, blockPosition);
			this->insertRecord(newData);
		}
	}

	if(newBucket != NULL)
	{
		delete newBucket;
		newBucket = NULL;
	}

	return success;
}

bool HashClasification::deleteRecord (Record* data)
{
	bool success = true;
	int blockPosition = this->hashKey(data->getKey());
	int blockNumber = this->hashTable->getBlock(blockPosition);
	Bucket* newBucket = this->readBucket(blockNumber);
	Record* record = newBucket->find(data->getKey());

	if(!record)
	{
		success = false;
	}
	else
	{
		//Obtiene los Datos de la lista
		int listNumber = record->getData()->readAsInt(0);
		int dataId = data->getData()->readAsInt(0);

		//Se quita el Id de la lista, si se libero algun bloque se agrega al file de bloques libres
		int countList = this->removeFromListofId(listNumber, dataId);

		//Si era el unico elemento de la lista, se elimina la key del hash
		if (countList == 0)
		{
			//Se quita el registro del bucket
			newBucket->deleteRecord(record->getKey());

			if(newBucket->isEmpty())
			{
				//Verifica si el bucket vacio puede liberado (pasado a freeBlockFile)
				int newBlock = this->hashTable->findDeletableBlock(blockPosition, newBucket->getDispersionNumber()/2);
				if(newBlock>=0)
				{
					//Actualiza todas las referencias que sea necesario de la tabla
					this->hashTable->updateReferences(blockPosition, newBucket->getDispersionNumber(), newBlock);

					//Se cambia el numero de dispersion del bucket
					Bucket* updateBucket = new Bucket(this->bucketSize);
					ByteString byteString = this->blockPersistor->readBlock(newBlock);
					updateBucket->Hidratate(byteString);
					updateBucket->setDispersionNumber(updateBucket->getDispersionNumber()/2);
					this->blockPersistor->saveBlock(newBlock, updateBucket->Serialize());
					delete updateBucket;

					//Funcion que se llama n veces hasta que la tabla no se pueda truncar mas
					while(this->hashTable->needTruncate());

					//si el bloque a liberar es el unico bucket, no se libera
					if(this->hashTable->getSize() > 1)
					{
						//Guarda el bucket como libre
						this->freeBlocks->insertFreeBlock(blockNumber);
						//Graba vacios en el archivo
						ByteString emptyString;
						emptyString.fillWithEmpty(this->bucketSize);
						this->blockPersistor->saveBlock(blockNumber, emptyString);
					}
					else
					{
						this->blockPersistor->saveBlock(blockNumber, newBucket->Serialize());
					}
				}
				else
				{
					this->blockPersistor->saveBlock(blockNumber, newBucket->Serialize());
				}
			}
			else
			{
				this->blockPersistor->saveBlock(blockNumber, newBucket->Serialize());
			}

		}
	}
//	delete newBucket;
//	delete data;
	return success;
}

list<int>* HashClasification::searchRecord (Key* key)
{
	list<int>* toReturn = NULL;

	int blockPosition = this->hashKey(key);
	int bucketNumber = this->hashTable->getBlock(blockPosition);
	Bucket* newBucket = this->readBucket(bucketNumber);

	Record* record = newBucket->find(key); //se libera al liberar el bucket (esta dentro de una lista)
	if(record)
	{
		//Si se encontro el id en el record, se obtiene la lista que corresponde
		int listNumber = record->getData()->readAsInt(sizeof(int)*0);
		int idTerm = record->getData()->readAsInt(sizeof(int)*1);

		//Recupera la lista
		toReturn = this->getToListofId(listNumber);

		//Inserta en la ultima posicion el id de termino (negativo, para evitar
		//no se produzca una duplicacion en la lista (solo deja uno)
		toReturn->push_back((-1)*idTerm);
	}
	else
	{
		//Devuelve la lista vacia
		toReturn = new list<int>();
	}

	//delete poddo
	if (newBucket != NULL)
		delete newBucket;

	return toReturn;
}

void HashClasification::printDataInFile (string path)
{
	ofstream o(path.c_str(),ios::out | ios::trunc);
	string str;

	o<<"------------------------------"<<endl;
	o<<"--------- Hash Table ---------"<<endl;
	o<<this->hashTable->TabletoString()<<endl;
	o<<this->hashTable->BlockstoString()<<endl;

	//Se cargan los bloques correspondientes a los buckets
	o<<"------------------------------"<<endl;
	o<<"----------- Buckets ----------"<<endl;
	Bucket bucket(this->blockPersistor->getBlockSize());
	ByteString byteString;

	set<int> bucketBlocks = this->hashTable->getBucketBlocks();
	set<int>::iterator blockIt;
	for(blockIt = bucketBlocks.begin(); blockIt!=bucketBlocks.end(); ++blockIt)
	{
		int blockNumber = *blockIt;

		byteString.clean();
		byteString.insertLast(this->blockPersistor->readBlock(blockNumber));
		bucket.Hidratate(byteString);
		o << "Bloque " << blockNumber << ": " << bucket.toString(true);

		//Imprime las listas invertidas de este bucket
		o << "Listas invertidas Bloque " << blockNumber << endl;
		ListIterator<Record*> recordIt = bucket.getRecordList()->getIterator();
		while(recordIt.hasNext())
		{
			//Obtiene el numero de bloque y lo guarda en la lista de bloques no buckets
			int blockList = recordIt.next()->getData()->readAsInt(0);

			//Muestra la lista
			ListofID* listofID = new ListofID(this->blockPersistor, this->freeBlocks, blockList, true);
			o << listofID->toString();
			delete listofID;
		}

		o << endl;
	}

	//Imprime bloque libres
	if(freeBlocks->isNotEmpty())
		o << freeBlocks->toString() << endl;

	o.close();
}

void HashClasification::exportInvertedList(string path)
{
	ofstream o(path.c_str(),ios::out | ios::trunc);
	string str;

	Bucket bucket(this->blockPersistor->getBlockSize());
	ByteString byteString;

	set<int> bucketBlocks = this->hashTable->getBucketBlocks();
	set<int>::iterator blockIt;
	for(blockIt = bucketBlocks.begin(); blockIt!=bucketBlocks.end(); ++blockIt)
	{
		int blockNumber = *blockIt;

		byteString.clean();
		byteString.insertLast(this->blockPersistor->readBlock(blockNumber));
		bucket.Hidratate(byteString);

		//Imprime las listas invertidas de este bucket
		o << "Listas invertidas Bloque " << blockNumber << endl;
		ListIterator<Record*> recordIt = bucket.getRecordList()->getIterator();
		while(recordIt.hasNext())
		{
			//Obtiene el numero de bloque y lo guarda en la lista de bloques no buckets
			Record* currentRecord = recordIt.next();
			int blockList = currentRecord->getData()->readAsInt(0);
			string term = currentRecord->getKey()->toString();

			o << "Termino: " << term <<  endl << "\t";
			//Muestra la lista
			ListofID* listofID = new ListofID(this->blockPersistor, this->freeBlocks, blockList, true);
			list<int>::iterator it;
			list<int> listIds = listofID->getListID();
			for(it = listIds.begin();it!=listIds.end();++it)
			{
				o << Utility::intToString(*it);
				o << ", ";
			}
			o << endl;

			delete listofID;
		}

		o << endl;
	}

	//Imprime bloque libres
	if(freeBlocks->isNotEmpty())
		o << freeBlocks->toString() << endl;

	o.close();
}

