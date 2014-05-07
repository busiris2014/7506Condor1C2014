#include "FileBlockManager.h"
#include "fstream"
#include "Constantes.h"

FileBlockManager::FileBlockManager(string path, int blockSize) {
	this->blockSize = blockSize;
	this->path = path;
	this->fileSize = -1;
}

FileBlockManager::~FileBlockManager() {
	this->fileClose();
}

bool FileBlockManager::fileNew(){
	bool response = true;
	FILE* resultado = fopen(path.c_str(),"w+");
	if(resultado)fclose(resultado);
		else response = false;
	return response;
}

bool FileBlockManager::fileOpen(){
	bool open;
	file.open(path.c_str(), ios::in | ios::out | ios::binary);
	open = file.is_open();
	if(open){
		int start,end;
		start = file.tellg();
		file.seekg(0,ios::end);
		end = file.tellg();
		fileSize = end - start;
	}
	return open;
}

void FileBlockManager::fileClose(){
	if(file.is_open()) file.close();
}

int FileBlockManager::fileRename(string newPath){
	bool open = file.is_open();
	if(open) file.close();
	int response = rename(this->path.c_str(),newPath.c_str());
	if(!response){
		this->path = newPath;
		if(open) file.open(path.c_str(), ios::in | ios::out | ios::binary);
	}

	return response;
}

int FileBlockManager::fileRemove(){
	return remove(this->path.c_str());
}

bool FileBlockManager::saveBlock (int position, ByteString byteString){
	return this->saveBlocks(position,1,byteString);
}

bool FileBlockManager::saveBlocks(int position,int n, ByteString byteString){
	bool response = false;
	int blocksSize = n * this->blockSize;

	//valida espacio del file
	if(((position) * blockSize + blocksSize) <= fileSize)
	{
		//valida listSize del string
		if(int(byteString.getSize()) == n*this->blockSize)
		{
			file.seekp(position * blockSize);
			file.write(byteString.toString().c_str(),byteString.getSize());
			response = true;
		}

	}
	return response;
}

ByteString FileBlockManager::readBlock (int position){
	return this->readBlocks(position,1);
}

ByteString FileBlockManager::readBlocks(int position,int n){
	ByteString blocks;
	int blocksSize = n * this->blockSize;

	//valida la posicion de lectura
	if(((position) * blockSize + blocksSize) <= fileSize)
	{
		file.seekg(position * blockSize);
		char buffer[blocksSize];
		file.read(buffer,blocksSize);
		blocks.insert(buffer,blocksSize,0);
	}
	return blocks;
}

void FileBlockManager::insertBlock (){
	this->insertBlocks(1);
}

void FileBlockManager::insertBlocks(int number){
	ByteString blocks;

	char empty = CONST_EMPTY;
	for(int i = 0; i< (blockSize*number);i++)
		blocks.insertLast(&empty,sizeof(char));

	file.seekp(fileSize);
	file.write(blocks.toString().c_str(), number*blockSize);
	fileSize = fileSize + number*blockSize;

}

int FileBlockManager::getFileSize(){
	return this->fileSize;
}

int FileBlockManager::getBlockCount(){
	return (this->fileSize / this->blockSize);
}

string FileBlockManager::getPath(){
	return this->path;
}

int FileBlockManager::getBlockSize()
{
	return this->blockSize;
}
