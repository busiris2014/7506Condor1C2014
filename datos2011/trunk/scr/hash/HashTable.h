#ifndef HASHTABLE_H_
#define HASHTABLE_H_
#include "Reference.h"
#include "../common/Key.h"
#include "../common/List.h"
#include "../common/FileBlockManager.h"
#include "../common/FreeBlockManager.h"
#include <vector>
#include <set>
#include <string>

using std::string;

class HashTable {

private:
	FreeBlockManager* freeBlocks;
	FileBlockManager* file;
	vector<int> blockList;
	vector<Reference*> referenceList;

	//Funcion que devuelve el un nuevo numero de bloque para utilizar
	int addNewBlock();

	//Funcion que carga en memoria una hashTable persistida en disco
	void loadHashTable();

	//Funcion que persiste en disco una hashTable cargada en memoria
	void saveHashTable();

	//Implementa el truncado de la tabla a la mitad
	bool truncateTable();

    // Setea el contenido de una referencia
    void setBlock(int indexReference, int position);

public:

	//Constructor
	HashTable(FileBlockManager* file, FreeBlockManager* freeBlocks);

	//Destructor
	virtual ~HashTable();

	//Duplica el tamanio de la tabla
	void duplicateTable(int referenceNumber, int newBucketNumber);

	//Analiza si puede truncar la tabla, y si puede la trunca (quedando la parte superior)
	bool needTruncate();

	//Actualiza las referencias en caso de una reestructuracion parcial
	void updateReferences(int initialReference, int displacement, int newBlockNumber);

    //Analiza si un bloque puede ser liberado. Si puede ser liberado, devuelve el nro de bloque, sino -1
    int findDeletableBlock(int initialPosition, int displacement);

    //Devuelve el valor contenido
    int getBlock(int indexReference);

    //Devuelve el tamanio de la tabla
	int getSize() const;

	//Devuelve la lista de bloques
	set<int> getBucketBlocks();

	string TabletoString();
	string BlockstoString();

};

#endif /* HASHTABLE_H_ */
