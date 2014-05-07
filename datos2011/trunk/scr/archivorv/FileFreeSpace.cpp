/*
 * FileFreeSpace.cpp
 *
 *  Created on: 30/03/2011
 *      Author: luciano
 */

#include "FileFreeSpace.h"
using namespace std;

FileFreeSpace::FileFreeSpace(string filaname) {
	this->fileName = filaname + ".dat";
	fileFreeSpaces.open(this->fileName.c_str(), ios::in | ios::out | ios::binary);
	if(fileFreeSpaces.is_open())
	{
		loadListFreeSpaces();
		fileFreeSpaces.close();
		remove(this->fileName.c_str());
	}
}

FileFreeSpace::~FileFreeSpace() {
	//Aca guardo los registros de la lista en el archvio.
	FILE* result = fopen(this->fileName.c_str(),"w+");
	if(result)
	{
		saveFile();
		empty();
		fileFreeSpaces.close();
	}
}

void FileFreeSpace::addFreeSpace(int position,int space)
{
	FreeSpace* freeSpace = new FreeSpace(position,space);
	listFreeSpaces.insertData(freeSpace);
}

int FileFreeSpace::searchBestSpace(int spaceNeeded)
{
	ListIterator<FreeSpace*> it = listFreeSpaces.getIterator();
	int position = -1;
	int differenceParam = 100000000;

	while(it.hasNext())
	{
		FreeSpace* space = it.next();
		int difference = (space->getFreeSpace() - spaceNeeded);
		if((difference < differenceParam)&&(difference>=0))
		{
			differenceParam = difference;
			position = space->getFilePosition();
		}
	}
	return position;
}

void FileFreeSpace::renameFile(string newPath)
{
	this->fileName = newPath;
}

FreeSpace* FileFreeSpace::getSpace(int spaceNeeded)
{
	FreeSpace* freeSpace = NULL;
	if(!listFreeSpaces.isEmpty())
	{
		int position = searchBestSpace(spaceNeeded);
		if(position!=-1)
		{
			bool found = false;
			FreeSpace* spaceIte;
			ListIterator<FreeSpace*> it= listFreeSpaces.getIterator();
			while(it.hasNext() && !found){
					spaceIte = it.next();
					if(spaceIte->getFilePosition() == position)
					{
						freeSpace = spaceIte;

						int remainingSpace = spaceIte->getFreeSpace()-spaceNeeded;
						int newPosition = spaceIte->getFilePosition()+spaceNeeded;
						if((newPosition>=0) && (remainingSpace>0))
								{
									FreeSpace* newSpace = new FreeSpace(newPosition,remainingSpace);
									listFreeSpaces.insertData(newSpace);
								}
						listFreeSpaces.removeData(spaceIte);
						found = true;
					}
			}
			if(!found) freeSpace = NULL;
		}
	}
	return freeSpace;
}

void FileFreeSpace::empty(){
	FreeSpace* freeSpace = NULL;
	while(!listFreeSpaces.isEmpty()){
		freeSpace = listFreeSpaces.getFirst();
		listFreeSpaces.removeData(freeSpace);
		delete freeSpace;
	}
}

void FileFreeSpace::saveFile()
{
	ListIterator<FreeSpace*> it = listFreeSpaces.getIterator();
	fileFreeSpaces.open(this->fileName.c_str(), ios::in | ios::out | ios::binary);
	int count = listFreeSpaces.getSize();
	fileFreeSpaces.write((char*)&count,NUMBER_SPACE);
	while(it.hasNext())
	{
		FreeSpace* freeSpace = it.next();
		int space = freeSpace->getFreeSpace();
		int position = freeSpace->getFilePosition();

		fileFreeSpaces.write((char*)&position,SIZE_REC_POS);

		fileFreeSpaces.write((char*)&space,SIZE_REC_SPACE);
	}
}

void FileFreeSpace::loadListFreeSpaces()
{
	fileFreeSpaces.seekg(0);
	char countSpaces[NUMBER_SPACE];
	fileFreeSpaces.read(countSpaces,NUMBER_SPACE);
	ByteString countOfRecords;
	countOfRecords.insertLast((void*)(countSpaces),NUMBER_SPACE);
	int size = countOfRecords.readAsInt(0);

	for(int i =0;i<size;i++)
	{
		char position[SIZE_REC_POS];
		fileFreeSpaces.read(position,SIZE_REC_POS);
		ByteString stringOfByte;
		stringOfByte.insertLast((void*)position,SIZE_REC_POS);
		char espacio[SIZE_REC_SPACE];
		fileFreeSpaces.read(espacio,SIZE_REC_SPACE);
		stringOfByte.insertLast((void*)espacio,SIZE_REC_SPACE);

		int integerPosition = stringOfByte.readAsInt(0);
		int integerSpace = stringOfByte.readAsInt(4);
		FreeSpace* freeSpace = new FreeSpace(integerPosition,integerSpace);
		listFreeSpaces.insertData(freeSpace);
	}
}


int FileFreeSpace::searchSpace(int position)
{
	ListIterator<FreeSpace*> it = listFreeSpaces.getIterator();
	int space = -1;
	FreeSpace* freeSpace;
	while(it.hasNext())
	{
		freeSpace = it.next();
		if(freeSpace->getFilePosition() == position)
			space = freeSpace->getFreeSpace();
	}
	return space;
}

