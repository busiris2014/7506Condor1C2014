#include "HashSelection.h"
#include "../common/Constantes.h"

HashSelection::HashSelection(string path): Hash(path)
{
}

HashSelection::~HashSelection()
{
}

bool HashSelection::insertRecord (Record* newData)
{
	bool success = true;
	int blockPosition = this->hashKey(newData->getKey());
	int bucketNumber = this->hashTable->getBlock(blockPosition);
	Bucket* newBucket = this->readBucket(bucketNumber);

	if(newBucket->find(newData->getKey()))
		success = false;
	else
	{
		//Si hay capacidad, se inserta en el bucket y se graba.
		//Sino se reestructura la tabla
		Record* record = newData->clone();
		if(newBucket->insertRecord(record))
			this->blockPersistor->saveBlock(bucketNumber, newBucket->Serialize());
		else
		{
			//libero el record si no se puedo insertar en el bucket,
			//si se inserto en el bucket, el liberar el bucket se liberan todos los registros
			delete record;

			//se resuelve el overflow del hash y se intenta de insertar nuevamente
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

bool HashSelection::deleteRecord (Record* data)
{
	bool success = true;
	int blockPosition = this->hashKey(data->getKey());
//	cout << data->getKey()->getKey() << endl;
	int blockNumber = this->hashTable->getBlock(blockPosition);
//	cout << blockNumber << endl;
	Bucket* newBucket = this->readBucket(blockNumber);
	Record* record = newBucket->find(data->getKey());

	if(!record)
	{
		success = false;
	}
	else
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
				ByteString byteString = this->blockPersistor->readBlock(newBlock);
				Bucket* updateBucket = new Bucket(this->bucketSize);
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
	return success;
}

list<int>* HashSelection::searchRecord (Key* key)
{
	list<int>* toReturn = new list<int>;
	Record* record = this->findRecord(key);
	if (record)
	{
		int id = record->getData()->readAsInt(0);
		toReturn->push_back(id);
	}

	delete record;
	return toReturn;
}

void HashSelection::printDataInFile (string path)
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
		o << endl;
	}

	//Imprime bloque libres
	if(freeBlocks->isNotEmpty())
		o << freeBlocks->toString() << endl;

	o.close();
}

