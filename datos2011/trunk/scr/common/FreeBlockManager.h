
#ifndef FREEBLOCKMANAGER_H_
#define FREEBLOCKMANAGER_H_
#include <string.h>
#include <fstream>
#include "../common/ByteString.h"
#include "../common/FileBlockManager.h"

class FreeBlockManager {
private:
	FileBlockManager* freeBlockFile;

	//Elimina el primer bloque de file de espacios libres
	void removeFirstBlock();
	//Elimina el lastNode bloque de file de espacios libres
	void removeLastBlock();
public:

	//Constructor
	FreeBlockManager(string path);
	//Destructor
	virtual ~FreeBlockManager();

	//Inserta un nuevo numero de bloque como bloque vacio
	void insertFreeBlock(int positionBlock);

	//Devuelve true si existe al menos un bloque vacio
	bool isNotEmpty();

	//Devuelve la cantidad de bloques vacios
	int getFreeBlockCount();

	//Devuelve el primer bloque vacio del file
	int getFirstFreeBlock();

	//Devuelve un bloque vacio del archivo
	int readFreeBlock(int block);

	//Devuelve una representacion del estado actual del file
	string toString();

};

#endif /* FREEBLOCKMANAGER_H_ */
