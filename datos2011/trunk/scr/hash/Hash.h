#ifndef HASH_H_
#define HASH_H_
#include "../common/Record.h"
#include "../common/Key.h"
#include "../common/FileBlockManager.h"
#include "../common/FreeBlockManager.h"
#include "../common/List.h"
#include "HashTable.h"
#include "Bucket.h"
using namespace std;

class Hash {
protected:
	HashTable* hashTable;
	FileBlockManager* blockPersistor;
	FreeBlockManager* freeBlocks;
	int bucketSize;

	//Esta funcion va a devolver el nro de un bloque disponible o uno nuevo
	int findNewBlock();

	//Transforma un string a un entero (usa como maximo los priemeros 4 caracteres)
	unsigned long hashString(string key);

	//Implementa la funcion de dispercion de la tabla de hash
	int hashKey(Key* key);

	//Redispersa los registros de un bucket entre 2 buckets
	void reHashRecords(Bucket* bucket, int bucketNumber, Bucket* newBucket);

	//Metodo que resuelve la logica de una insercion que da overflow
	void resolveBucketOverflow(Bucket* overflowedBucket, int bucketNumber, int referenceNumber);

public:
	//Constructor
	Hash(string path);

	//Destructor
	virtual ~Hash();

	//Devuelve el bucket contenido en un bloque particular
	Bucket* readBucket(int blockNumber);

	//Devuelve un registro, buscandolo a travez de la clave (clave - id o nrobloque de la lista invertida)
	Record* findRecord (Key* key);

	//METODOS ABSTRACTOS
	//Metodo abstracto, debe implementar la insercion de un nuevo registro al hash (seleccion o clasificacion)
	virtual bool insertRecord (Record* newData) = 0;

	//Metodo abstracto, debe implementar la eliminacion de un registro del hash (seleccion o clasificacion)
	virtual bool deleteRecord (Record* data) = 0;

	//Metodo abstracto, debe implementar la busqueda de un registro del hash (seleccion o clasificacion)
	//virtual List<int>* searchRecord (Key* key) = 0;

	//Metodo abstracto, debe implementar la impresion del estado actual del hash
	virtual void printDataInFile (string path) = 0;

};

#endif /* HASH_H_ */
