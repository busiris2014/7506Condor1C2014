#ifndef HASHSELECTION_H_
#define HASHSELECTION_H_
#include "Bucket.h"
#include "Hash.h"
#include <set>
#include <list>
using namespace std;

class HashSelection : public Hash {
private:

public:
	//Constructor
	HashSelection(string path);

	//Destructor
	virtual ~HashSelection();

	bool insertRecord (Record* newData);

	bool deleteRecord (Record* data);

	list<int>* searchRecord (Key* key);

	void printDataInFile (string path);
};

#endif /* HASHSELECTION_H_ */
