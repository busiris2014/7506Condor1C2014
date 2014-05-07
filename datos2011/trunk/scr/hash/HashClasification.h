#ifndef HASHCLASIFICATION_H_
#define HASHCLASIFICATION_H_
#include "Hash.h"
#include "../common/ListofID.h"
#include <set>
#include <list>

class HashClasification: public Hash {
private:
	//Se un referencia a la lista particular
	list<int>* getToListofId(int listNumber);
	//Se agrega un nuevo id a una lista particular y se graba
	bool addToListofId(int listNumber, int newDataId);
	//Se elimina un nuevo id a una lista particular, se graba,
	//devuelve la cantidad de elementos que quedaron en la lista
	int removeFromListofId(int listNumber, int dataId);

public:
	HashClasification(string path);

	//Destructor
	virtual ~HashClasification();

	bool insertRecord (Record* newData);

	bool deleteRecord (Record* data);

	list<int>* searchRecord (Key* key);

	void printDataInFile (string path);

	void exportInvertedList(string path);

};

#endif /* HASHCLASIFICATION_H_ */
