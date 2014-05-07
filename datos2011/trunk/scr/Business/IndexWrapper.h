#ifndef INDEX_WRAPPER_H_
#define INDEX_WRAPPER_H_

#include <iostream>
#include <string>
#include <list>
#include "../btree/ClassifBPlusTree.h"
#include "../btree/BPlusTreeIterator.h"
#include "../hash/HashClasification.h"
#include "../hash/HashSelection.h"
#include "../common/Key.h"
#include "../common/Book.h"
#include "../parser/FileParser.h"

using namespace std;

class IndexWrapper {

public:

	/**
	 * singleton
	 */
	static IndexWrapper* getInstanceOfIndexWrapper();

	/**
	 * Enumerado para cada indice de los que estan contenidos
	 * (despues habria que refactorizar a un facade)
	 */
	static enum indexItem {EDITORIAL = 0, AUTOR = 1, TITULO = 2, PALABRAS = 3} tIndexItem;

	/*
	 * crea y guarda un indice del tipo especificado por el enumerado con el nombre especificado
	 */
	void createIndex(string path, indexItem itemType, int keyTopSize = 24);

	/*
	 * cierra un indice especificado
	 */
	void closeIndex(indexItem itemType);

	/**
	 * agrega un registro al indice especificado
	 */
	bool add(Record* r, indexItem itemType);

	/**
	 * remueve un registro del indice especificado
	 */
	bool remove(Record* r, indexItem itemType);

	/**
	 * busca un registro en el indice especificado
	 */
	Record* search(Key* key, indexItem itemType);

	/**
	 * busca la lista de ids de una determinada clave
	 */
	list<int>* searchAllIds(Key* k, indexItem itemType);

	/**
	 * retorna true si pudo eliminar el libro de todos los indices, si al menos uno, falló, devuelve false
	 */
	bool removeFromAll(Book b);

	/**
	 * destructor, si son distintos de null los indices contenidos hace un free
	 */
	virtual ~IndexWrapper();

	/**
	 * exporta a texto plano el indice especificado por el 2do parametro, en el path "path"
	 */
	void exportIndex(string path, indexItem type);

	void exportInvertedList(string path);

protected:

	IndexWrapper();

private:

	ClassifBPlusTree* editorial; // FIXME pide un facade a gritos
	ClassifBPlusTree* author;
	HashSelection* tittle;
	HashClasification* words;
	static IndexWrapper* instance;


};

#endif /* INDEX_WRAPPER_H_ */
