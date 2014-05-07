#include "Hash.h"
#include "HashTable.h"
#include "Bucket.h"
//#include "../common/Constantes.h"
#include "../common/Resource.h"


//#define DIVISOR_A 101
//#define DIVISOR_B 501

Hash::Hash(string path)
{
	//lee la configuracion
	this->bucketSize = Resource::getConfigProperties()->getAsInt("booquerio.hash.bucket.tamanio");

	this->freeBlocks = new FreeBlockManager(path + ".free");
	this->blockPersistor = new FileBlockManager(path + ".hash", bucketSize);
	this->hashTable = new HashTable(this->blockPersistor, this->freeBlocks);

	//Si el archivo solo tiene un bloque, quiere decir que solo contiene
	//el bloque del hashTable. Por lo tanto hay que inicializar un bucket.
	if(this->blockPersistor->getBlockCount() == 1)
	{
		this->blockPersistor->insertBlock();
		Bucket firstBucket(bucketSize);
		this->blockPersistor->saveBlock(1, firstBucket.Serialize());
	}
}

Hash::~Hash()
{
	if (this->hashTable) delete this->hashTable;
	if (this->blockPersistor) delete this->blockPersistor;
	if (this->freeBlocks) delete this->freeBlocks;
}

//SDBM:
//this algorithm was created for sdbm (a public-domain reimplementation of ndbm) database library.
//it was found to do well in scrambling bits, causing better distribution of the keys and fewer splits.
//it also happens to be a good general hashing function with good distribution.
//the actual function is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below is the faster version used in gawk.
//[there is even a faster, duff-device version] the magic constant 65599 was picked out of thin air while experimenting
//with different constants, and turns out to be a prime. this is one of the algorithms used in berkeley db (see sleepycat) and elsewhere.
unsigned long Hash::hashString(string key)
{
    unsigned long hash = 0;
    int c;

    //for (unsigned int i = 0; i < key.length() && i < 5; i++)
	for (unsigned int i = 0; i < key.length(); i++)
    {
    	c = key[i];
    	hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

int Hash::hashKey(Key* key)
{
	unsigned long n = this->hashString(key->getKey());
	n = n % (unsigned int)this->hashTable->getSize();
	return n;
}

int Hash::findNewBlock()
{
	int newBlockNumber = this->freeBlocks->getFirstFreeBlock();
	if(newBlockNumber < 0){
		this->blockPersistor->insertBlock();
		newBlockNumber = this->blockPersistor->getBlockCount() - 1;
	}

	return newBlockNumber;
}

void Hash::reHashRecords(Bucket* bucket, int bucketNumber, Bucket* newBucket)
{
	List<Record*>* recordList = bucket->getRecordList();
	ListIterator<Record*> it = recordList->getIterator();
	Record* auxRecord = NULL;
	int bucketDestinationNumber = 0;
	while(it.hasNext()){
		auxRecord = it.next();
		bucketDestinationNumber = this->hashTable->getBlock(this->hashKey(auxRecord->getKey()));
		if(bucketDestinationNumber != bucketNumber){
			newBucket->insertRecord(auxRecord->clone());
			bucket->deleteRecord(auxRecord->getKey());
		}
	}
}

void Hash::resolveBucketOverflow(Bucket* overflowedBucket, int bucketNumber, int referenceNumber)
{
	//se crea un nuevo bucket
	Bucket* newBucket = new Bucket(this->bucketSize);
	int newBucketNumber = this->findNewBlock();

	//Actualiza el factor de dispersion
	int originalDispersionNumber = overflowedBucket->getDispersionNumber();
	overflowedBucket->setDispersionNumber(2 * originalDispersionNumber);
	newBucket->setDispersionNumber(2 * originalDispersionNumber);

	bool duplicate = (originalDispersionNumber == this->hashTable->getSize());
	if(duplicate)
		this->hashTable->duplicateTable(referenceNumber, newBucketNumber);
	else
		this->hashTable->updateReferences(referenceNumber, overflowedBucket->getDispersionNumber(), newBucketNumber);

	this->reHashRecords(overflowedBucket, bucketNumber, newBucket);
	this->blockPersistor->saveBlock(bucketNumber, overflowedBucket->Serialize());
	this->blockPersistor->saveBlock(newBucketNumber, newBucket->Serialize());
	delete newBucket;
}

//Metodos Publicos

Bucket* Hash::readBucket(int bucketNumber){
	ByteString byteString = this->blockPersistor->readBlock(bucketNumber);

	Bucket* bucket = new Bucket(this->bucketSize);
	bucket->Hidratate(byteString);
	return bucket;
}

Record* Hash::findRecord(Key* key){
	int bucketNumber = this->hashTable->getBlock(this->hashKey(key));
	Bucket* bucket = this->readBucket(bucketNumber);

	Record* record = bucket->find(key);
	if(record)
		record = record->clone();

	delete bucket;
	return record;
}
