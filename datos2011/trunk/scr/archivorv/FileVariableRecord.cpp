#include "FileVariableRecord.h"
#include "fstream"
#include "../common/Constantes.h"
#include <iostream>
#include "../common/Record.h"
#include "../common/Key.h"
#include "../common/ByteString.h"
#include "FileFreeSpace.h"
#include "../common/Resource.h"

using namespace std;

FileVariableRecord::FileVariableRecord(string path) {
	string controlPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.control");

	this->path = controlPath + path + ".rev";
	this->fileSize = -1;
	this->fileFreeSpace = new FileFreeSpace(controlPath + path);
}

FileVariableRecord::~FileVariableRecord() {
	this->closeFile();
	delete fileFreeSpace;
}

bool FileVariableRecord::createFile(){
	bool success = true;
	FILE* result = fopen(path.c_str(),"w+");
	if(result)fclose(result);
		else success = false;
	return success;
}

bool FileVariableRecord::openFile(){
	bool open;
	file.open(path.c_str(), ios::in | ios::out | ios::binary);
	open = file.is_open();
	if(open){
		int inicio,final;
		inicio = file.tellg();
		file.seekg(0,ios::end);
		final = file.tellg();
		fileSize = final - inicio;
	}
	this->actualPosition = 0;
	return open;
}

void FileVariableRecord::closeFile(){
	if(file.is_open()) file.close();
}

int FileVariableRecord::renameFile(string path){
	bool open = file.is_open();
	if(open) file.close();
	int retorno = rename(this->path.c_str(),path.c_str());
	if(!retorno){
		this->path = path;
		if(open)
		{
			file.open(path.c_str(), ios::in | ios::out | ios::binary);
			fileFreeSpace->renameFile(path);
		}

	}

	return retorno;
}

int FileVariableRecord::deleteFile(){
	return remove(this->path.c_str());
}


int FileVariableRecord::getFileSize(){
	return this->fileSize;
}

string FileVariableRecord::getPath(){
	return this->path;
}


int FileVariableRecord::addBook(ByteString byteString)
{
	int filePosition = -1;
	if(file.is_open())
	{
		FreeSpace* space = fileFreeSpace->getSpace(byteString.toString().size());
		//falta la metadata del campo baja logica
		if(space!=NULL)
		{
			file.seekp(space->getFilePosition());
			int recordSize = byteString.toString().size();
			file.write((char*)&recordSize,SIZE_LONG_VAR_REC);
			file.write((char*)byteString.toString().c_str(),byteString.toString().size());
			filePosition = space->getFilePosition();
		}
		else
		{
			file.seekp(this->getFileSize());
			int recordSize = byteString.toString().size();
			file.write((char*)&recordSize,SIZE_LONG_VAR_REC);
			file.write((char*)byteString.toString().c_str(),byteString.toString().size());
			filePosition = this->fileSize;
			this->fileSize += byteString.toString().size() + SIZE_LONG_VAR_REC;
		}
		delete space;
	}
	return filePosition;
}

void FileVariableRecord::deleteBook(int position)
{
	int recordSize = getBookSize(position);

	file.seekp(position+SIZE_LONG_VAR_REC);
	this->fileFreeSpace->addFreeSpace(position,recordSize);
}

int FileVariableRecord::getBookSize(int position)
{
	file.seekp(position);
	char recordSize[SIZE_LONG_VAR_REC];
	file.read(recordSize,SIZE_LONG_VAR_REC);
	ByteString cadena;
	cadena.insertLast((void*)recordSize,SIZE_LONG_VAR_REC);
	int sizeRec = cadena.readAsInt(0);

	return sizeRec;
}

ByteString FileVariableRecord::readBook(int position)
{
	int space = fileFreeSpace->searchSpace(position);
	ByteString byteString;
	if(space < 0)
	{
		int recordSize = getBookSize(position);
		char record[recordSize];
		file.seekp(position+SIZE_LONG_VAR_REC);
		file.read((char*)&record,recordSize);

		byteString.insertLast((void*)record,recordSize);
	}
	return byteString;
}

ByteString FileVariableRecord::readNext()
{
	bool found;
	ByteString bs;
	while (!found)
	{
		int space = fileFreeSpace->searchSpace(this->actualPosition);
		if(space>-1)
			this->actualPosition += SIZE_LONG_VAR_REC+space;
		else
		{
			if(this->actualPosition >= this->fileSize)
			{
				this->actualPosition = 0;
				break;
			}
			else
			{
				bs = readBook(this->actualPosition);
				this->actualPosition += SIZE_LONG_VAR_REC + bs.toString().size();
				found = true;
			}
		}
	}
	return bs;
}
