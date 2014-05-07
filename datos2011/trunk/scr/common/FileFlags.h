/*
 * FileFlags.h
 *
 *  Created on: 22/04/2011
 *      Author: luciano
 */

#ifndef FILEFLAGS_H_
#define FILEFLAGS_H_
#include <fstream>
#include "../common/List.h"
#include "../common/ByteString.h"
#define NUMBER_OF_IDS sizeof(int)
#define ID_NUMBER sizeof(int)

class FileFlags {
private:
	string fileName;
	List<int> listOfID;
	fstream fileFlag;
	void saveFile();
	void loadListOfIDs();
	void emptyList();

public:
	FileFlags(string filepath);
	bool addID(int id);
	List<int>* getListID();
	bool removeID(int ID);
	virtual ~FileFlags();
};

#endif /* FILEFLAGS_H_ */
