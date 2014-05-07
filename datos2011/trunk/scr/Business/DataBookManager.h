#ifndef DATA_BOOK_MANAGER_H_
#define DATA_BOOK_MANAGER_H_

#include "../archivorv/FileVariableRecord.h"
#include "IndexWrapper.h"
#include "../common/FileFlags.h"
#include <iostream>
#include "../common/Book.h"

using namespace std;

class DataBookManager {

private:

	FileVariableRecord* fileVariableRecord;
	FileFlags* fileFlagsEditorial;
	FileFlags* fileFlagsAutor;
	FileFlags* fileFlagsTitulos;
	FileFlags* fileFlagsPalabras;
	BPlusTree* principalIndex;
	fstream fileAutoIncInteger;
	int autoIncInteger;

public:

	DataBookManager(string fileNameVR,int maxKeySizeBPTree = sizeof(int));
	/*
	 * Agrega al archivo de flags correspondiente el ID siempre que no exista devolviendo true, sino devuelve false
	 * Pre: El archivo debe estar creado y abierto
	 * Post: Devuelve la lista de IDs contenido
	 */
	bool addIDtoFileFlags(IndexWrapper::indexItem item,int ID);
	/*
	 * Vacia el archivo correspondiente al index indicado y devuelve la lista de IDs contenido
	 * Pre: El archivo debe estar creado y abierto
	 * Post: Devuelve la lista de IDs contenido
	 */
	List<int>* getListToFileFlag(IndexWrapper::indexItem item);
	/*
	 * Agrega el record pasado por parametro al archivo de registros variables
	 * Pre: El archivo debe estar creado y abierto
	 * Post: Devuelve la posicion donde se guardo el registro en el archivo de registros variables o -1 si no se agrego
	 */
	bool addBook(ByteString byteString);

	bool removeRecord(int ID);

	bool removeIDtoFileFlags(IndexWrapper::indexItem item,int ID);

	ByteString getBook(int ID);

	string getlistOfTakenBooks();

	~DataBookManager();

};

#endif /* DATA_BOOK_MANAGER_H_ */
