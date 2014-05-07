/*
 * FileBlockNodeMapper.cpp
 *
 *  Created on: 21/04/2011
 *      Author: luciano
 */

#include "FileBlockNodeMapper.h"

FileBlockNodeMapper::FileBlockNodeMapper(string filaname) {
	this->fileName = filaname + ".map";
	fileBlockNodeRecords.open(this->fileName.c_str(), ios::in | ios::out | ios::binary);
	if(fileBlockNodeRecords.is_open())
	{
		loadListBlockNodeRecords();
		fileBlockNodeRecords.close();
		remove(this->fileName.c_str());
	}
}

FileBlockNodeMapper::~FileBlockNodeMapper(){
	//Aca guardo los registros de la lista en el archvio.
	FILE* result = fopen(this->fileName.c_str(),"w+");
	if(result)
	{
		saveFile();
		empty();
		fileBlockNodeRecords.close();
	}
}

void FileBlockNodeMapper::addBlockNodeRecord(int node,int block)
{
	BlockNodeRecord* blockNodeRecord = new BlockNodeRecord(node,block);
	listBlockNodeRecods.insertData(blockNodeRecord);
}

void FileBlockNodeMapper::renameFile(string newPath)
{
	this->fileName = newPath;
}

void FileBlockNodeMapper::empty(){
	BlockNodeRecord* blockNodeRecord = NULL;
	while(!listBlockNodeRecods.isEmpty()){
		blockNodeRecord = listBlockNodeRecods.getFirst();
		listBlockNodeRecods.removeData(blockNodeRecord);
		delete blockNodeRecord;
	}
}

void FileBlockNodeMapper::saveFile()
{
	ListIterator<BlockNodeRecord*> it = listBlockNodeRecods.getIterator();
	bool found = false;
	fileBlockNodeRecords.open(this->fileName.c_str(), ios::in | ios::out | ios::binary);
	int count = listBlockNodeRecods.getSize();
	fileBlockNodeRecords.write((char*)&count,NUMBER_NODES);
	while(it.hasNext() && ! found)
	{
		BlockNodeRecord* blockNodeRecord = it.next();
		int node = blockNodeRecord->getNode();
		int block = blockNodeRecord->getBlock();

		fileBlockNodeRecords.write((char*)&node,SIZE_NODE);

		fileBlockNodeRecords.write((char*)&block,SIZE_BLOCK);
	}
}

void FileBlockNodeMapper::loadListBlockNodeRecords()
{
	fileBlockNodeRecords.seekg(0);
	char countSpaces[NUMBER_NODES];
	fileBlockNodeRecords.read(countSpaces,NUMBER_NODES);
	ByteString countOfRecords;
	countOfRecords.insertLast((void*)(countSpaces),NUMBER_NODES);
	int size = countOfRecords.readAsInt(0);

	for(int i =0;i<size;i++)
	{
		char position[SIZE_NODE];
		fileBlockNodeRecords.read(position,SIZE_NODE);
		ByteString stringOfByte;
		stringOfByte.insertLast((void*)position,SIZE_NODE);
		char block[SIZE_BLOCK];
		fileBlockNodeRecords.read(block,SIZE_BLOCK);
		stringOfByte.insertLast((void*)block,SIZE_BLOCK);

		int integerNode = stringOfByte.readAsInt(0);
		int integerBlock = stringOfByte.readAsInt(4);
		BlockNodeRecord* blockNodeRecord = new BlockNodeRecord(integerNode,integerBlock);
		listBlockNodeRecods.insertData(blockNodeRecord);
	}
}

int FileBlockNodeMapper::getBlock(int nodo)
{
	ListIterator<BlockNodeRecord*> it = listBlockNodeRecods.getIterator();
	BlockNodeRecord* blockNodeRecord;

	int block = -1;
	bool found = false;
	while(it.hasNext() && !found)
	{
		blockNodeRecord = it.next();
		if(blockNodeRecord->getNode() == nodo)
		{
			found = true;
			block = blockNodeRecord->getBlock();
		}
	}
	return block;
}

int FileBlockNodeMapper::getSizeList()
{
	return listBlockNodeRecods.getSize();
}

/*
int main(int argc, char** argv) {
	FileBlockNodeMapper* fileBlockNodeMapper = new FileBlockNodeMapper("archivo");
	fileBlockNodeMapper->addBlockNodeRecord(30,10);
	fileBlockNodeMapper->addBlockNodeRecord(100,20);
	fileBlockNodeMapper->addBlockNodeRecord(1000,24);
	fileBlockNodeMapper->addBlockNodeRecord(345,13);
	int block = fileBlockNodeMapper->getBlock(345);
	cout<<block<<endl;
	delete fileBlockNodeMapper;
}*/


