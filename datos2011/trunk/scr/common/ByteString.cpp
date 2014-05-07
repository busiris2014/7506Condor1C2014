#include "ByteString.h"
#include "../Utility/Utility.h"

ByteString::ByteString() {}

ByteString::ByteString(string bytes)
{
	this->bytes = bytes;
}

ByteString::ByteString(const ByteString &byteString) {
	this->bytes = byteString.bytes;
}

ByteString::ByteString (void* value, int size)
{
	insert(value, size, 0);
}

ByteString::~ByteString() {}

bool ByteString::isEmpty() const {
	return (this->getSize() == 0);
}

string ByteString::toString() const {
	return this->bytes;
}

bool ByteString::insert(const ByteString& bytes, unsigned int position) {
	bool exito = true;
	if (position > this->bytes.length()) {
		exito = false;
	}else{
		this->bytes.insert(position,bytes.toString());
	}
	return exito;
}

bool ByteString::insert(const string &bytes, unsigned int position) {
	bool exito = true;
	if (position > this->bytes.length()){
		exito = false;
	}else{
		this->bytes.insert(position,bytes);
	}
	return exito;
}

bool ByteString::insert(void* value, unsigned int size, unsigned int position){
	bool exito = true;
		string cadena;
		char buffer[size];
		memcpy(buffer,value,size);
		cadena.append(buffer,size);
		exito = insert(cadena,position);
	return exito;
}

bool ByteString::insertLast(const string &bytes) {
	this->bytes.append(bytes);
	return true;
}

bool ByteString::insertLast(const ByteString& bytes) {
	bool exito = insert(bytes,getSize());
	return exito;
}

bool ByteString::insertLast(void* value, int size){
	bool exito = insert(value,size,this->getSize());
	return exito;
}

bool ByteString::replace(const string &bytes, unsigned int position) {
	bool exito = true;
	if ((position + bytes.length()) > this->getSize()) {
		exito = false;
	}else {
		this->remove(position, position + bytes.length() -1);
		this->insert(bytes, position);
	}
	return exito;
}

bool ByteString::replace(const ByteString &byteString, unsigned int position){
	bool exito = true;
	exito = replace(byteString.toString(),position);
	return exito;
}

bool ByteString::remove(unsigned int start, unsigned int end) {
	bool exito = true;
	if ((start > end) || ((end > this->bytes.length()) || ( end > this->bytes.length()))) {
		exito = false;
	}else{
		this->bytes.erase(start,end - start + 1);
	}
	return exito;
}

bool ByteString::read(char* buffer, unsigned int start, unsigned int size)const {
	bool exito = true;
	ByteString aux = this->read(start,size);
	if(aux.getSize()==0){
		exito = false;
	}else{
		memcpy(buffer,aux.bytes.data(),size);
		exito = true;
	}
	return exito;
}

ByteString ByteString::read(unsigned int start, unsigned int size) const {
	if (start + size > getSize()) {
		return ByteString("");
	}
	return this->bytes.substr(start,size);
}

int ByteString::readAsInt(unsigned int start){
	int n = -1;
	char buffer[sizeof(int)];
	bool leido = this->read(buffer,start,sizeof(int));
	if(leido) memcpy(&n,buffer,sizeof(int));
	return n;
}

bool ByteString::clean() {
	this->bytes.clear();
	return true;
}

unsigned int ByteString::getSize() const {
	return this->bytes.length();
}

void ByteString::setByteString(const string &bytes) {
	this->bytes = bytes;
}

bool ByteString::fillWithEmpty(unsigned int finalSize)
{
	bool success = false;
    if (this->getSize() < finalSize)
    {
        int count = finalSize - this->getSize();
        string empty(count, CONST_EMPTY);
        this->insertLast(empty);
        success = true;
    }

    return success;
    /*bool success = false;
    if (this->getSize() < finalSize)
    {
    	int count = finalSize - this->getSize();
        char empty = CONST_EMPTY;

        for(int i = 0; i < count; i++)
            this->insertLast(&empty, sizeof(char));

        success = true;
    }

    return success;*/
}
