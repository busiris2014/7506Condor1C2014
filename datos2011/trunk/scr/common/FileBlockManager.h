
#ifndef FILEBLOCKMANAGER_H_
#define FILEBLOCKMANAGER_H_
#include "../common/ByteString.h"
#include <string.h>
#include <fstream>

class FileBlockManager {
private:
	fstream file;
	string path;
	int blockSize;
	int fileSize;//indica el tamaño del file, con -1 se indica que no esta abierto

public:

	//Constructor
	FileBlockManager(string path, int blockSize);

	//Destructor
	virtual ~FileBlockManager();

	//Crea un nuevo file
	bool fileNew ();

	//Abre un file existente
	bool fileOpen ();

	//Cierra un file
	void fileClose();

	//Renombra un file (no cambia el estado del file)
	int fileRename(string newPath);

	//Borra un file
	int fileRemove();

	//Graba un bloques en el file abierto
	bool saveBlock (int position, ByteString byteString);

	//Graba n bloques consecutivos
	bool saveBlocks(int position, int number, ByteString byteString);

	//Lee un bloque determinado de el file
	ByteString readBlock (int position);

	//Lee n bloques consecutivos
	ByteString readBlocks(int position, int number);

	//Inserta un bloque vacio en el file
	void insertBlock ();

	//Inserta n bloques vacios en el file
	void insertBlocks(int n);

	//Devuelve el listSize del file
	int getFileSize();

	//Devuelve la cantidad de bloques en el file
	int getBlockCount();

	string getPath();

	int getBlockSize();
};

#endif /* FILEBLOCKMANAGER_H_ */
