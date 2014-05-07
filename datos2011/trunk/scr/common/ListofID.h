#ifndef LISTOFID_H_
#define LISTOFID_H_
#include <list>
#include "ByteString.h"
#include "Serializer.h"
#include "FileBlockManager.h"
#include "FreeBlockManager.h"
#include "Constantes.h"
#include "gammaCoder.h"

class ListofID
{

public:
	list<int> listOfID;
	list<int> listOfBlock;
	FileBlockManager* fileBlockManager;
	FreeBlockManager* freeBlockManager;
	int countBlock;

public:
	//lee del archvo pasado por parametro (fileBlockManager) los bloques donde esta contenida la lista y genera la listOfID
	ListofID(FileBlockManager* fileBlockManager, int idBlockBegin);
	ListofID(FileBlockManager* fileBlockManager, FreeBlockManager* freeBlockManager, int idBlockBegin);
	ListofID(FileBlockManager* fileBlockManager, FreeBlockManager* freeBlockManager, int idBlockBegin,bool compressed);
	virtual ~ListofID();

	//agrega a la lista el ID en caso de ya existir devuelve false
	bool addID(int ID);
	//elimina de la lista el ID
	bool removeID(int ID);
	list<int>* cloneListID();

	//devuelve la lista de IDs
	list<int> getListID();
	//Devuelve una representacion de la lista
	string toString();
	string toString(bool decompress);
	// escribe la lista en los bloques que correspondan, si debe pedir mas se los pide al fileBlockManager
	// y si libero devuelve el numero de bloque liberado
	int writeListinFile();
	int writeListinFile(bool compressed);
private:
	//Funcion que carga en memoria una hashTable persistida en disco
	void readListinFile(int idBlockBegin);

	//Funcion que devuelve el un nuevo numero de bloque para utilizar
	int getNewBlock();

	//Funcion ingresa un nuevo bloque como bloque libre
	int setFreeBlock(int block);

	void compress();
	void decompress();

};

#endif /* LISTOFID_H_ */
