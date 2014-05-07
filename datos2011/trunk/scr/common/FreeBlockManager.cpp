
#include "FreeBlockManager.h"
#include "../Utility/Utility.h"

FreeBlockManager::FreeBlockManager(string path) {
	this->freeBlockFile = new FileBlockManager(path, sizeof(int));
	if(!this->freeBlockFile->fileOpen()){
		this->freeBlockFile->fileNew();
		this->freeBlockFile->fileOpen();
	}
}

void FreeBlockManager::removeFirstBlock(){
	if(this->freeBlockFile->getBlockCount() > 0){
		FileBlockManager* newFreeBlockFile = new FileBlockManager(this->freeBlockFile->getPath() + ".temp",sizeof(int));
		newFreeBlockFile->fileNew();
		newFreeBlockFile->fileOpen();
		ByteString content;
		content.insertLast(this->freeBlockFile->readBlocks(1,this->freeBlockFile->getBlockCount()-1));
		newFreeBlockFile->insertBlocks(this->freeBlockFile->getBlockCount()-1);
		newFreeBlockFile->saveBlocks(0,this->freeBlockFile->getBlockCount() -1,content);
		this->freeBlockFile->fileClose();
		this->freeBlockFile->fileRemove();
		newFreeBlockFile->fileRename(this->freeBlockFile->getPath());
		delete this->freeBlockFile;
		this->freeBlockFile = newFreeBlockFile;
	}
}

void FreeBlockManager::removeLastBlock(){
	if(this->freeBlockFile->getBlockCount() > 0){
		FileBlockManager* newFreeBlockFile = new FileBlockManager(this->freeBlockFile->getPath() + ".temp",sizeof(int));
		newFreeBlockFile->fileNew();
		newFreeBlockFile->fileOpen();
		ByteString content;
		content.insertLast(this->freeBlockFile->readBlocks(0,this->freeBlockFile->getBlockCount()-1));
		newFreeBlockFile->insertBlocks(this->freeBlockFile->getBlockCount()-1);
		newFreeBlockFile->saveBlocks(0,this->freeBlockFile->getBlockCount() -1,content);
		this->freeBlockFile->fileClose();
		this->freeBlockFile->fileRemove();
		newFreeBlockFile->fileRename(this->freeBlockFile->getPath());
		delete this->freeBlockFile;
		this->freeBlockFile = newFreeBlockFile;
	}
}

void FreeBlockManager::insertFreeBlock(int positionBlock){
	this->freeBlockFile->insertBlock();
	ByteString byteString;
	byteString.insertLast(&positionBlock,sizeof(int));
	this->freeBlockFile->saveBlock(this->freeBlockFile->getBlockCount()-1,byteString);
}

bool FreeBlockManager::isNotEmpty(){
	return (this->freeBlockFile->getBlockCount() > 0);
}

int FreeBlockManager::getFreeBlockCount(){
	return this->freeBlockFile->getBlockCount();
}

int FreeBlockManager::getFirstFreeBlock(){
	int freeBlock;

	if (this->freeBlockFile->getBlockCount() < 1)
	{
		freeBlock = -1;
	}
	else
	{
		freeBlock = this->freeBlockFile->readBlock(this->freeBlockFile->getBlockCount()-1).readAsInt(0);
		this->removeLastBlock();
	}
	return freeBlock;
}

int FreeBlockManager::readFreeBlock(int block)
{
	int freeBlock;

	if (this->freeBlockFile->getBlockCount()-1 < block)
		freeBlock = -1;
	else
		freeBlock = this->freeBlockFile->readBlock(block).readAsInt(0);

	return freeBlock;
}

string FreeBlockManager::toString(){
	string cadena("Bloques libres: ");
	for(int i =0;i<this->freeBlockFile->getBlockCount(); i++){
		cadena += Utility::intToString(this->freeBlockFile->readBlock(i).readAsInt(0)) + " ";
	}
	cadena += "\n";
	return cadena;
}

FreeBlockManager::~FreeBlockManager() {
	this->freeBlockFile->fileClose();
	delete this->freeBlockFile;
}
