#include "Key.h"
#include "Constantes.h"
using std::string;

Key::Key()
{
	this->key = "";
}

Key::Key(string key)
{
	this->key = key;
}

Key::~Key() {

}

string Key::toString() const{
	return this->key;
}

ByteString Key::Serialize()
{
	ByteString byteString;
	char size = this->getSize();
	byteString.insertLast(&size,TAM_LONG_CLAVE);
	byteString.insertLast(this->key);
	return byteString;
}

bool Key::Hidratate(ByteString &byteString){
	bool response;
	if (byteString.getSize() < TAM_LONG_CLAVE){
		response = false;
	}else{
		this->key = "";
		if(byteString.getSize() > TAM_LONG_CLAVE){
			char tamano;
			byteString.read(&tamano,0,TAM_LONG_CLAVE);
			int size = int(tamano);
			char buffer[size];
			byteString.read(buffer,TAM_LONG_CLAVE,size);
			this->key.append(buffer,size);
		}
		response = true;
	}
	return response;
}

ByteString Key::SerializeEncoded(FrontCoder &encoder)
{
	ByteString byteString;
	string keyaux = this->key;
	this->key=encoder.encode(this->key);
	char size = this->getSize();
	byteString.insertLast(&size,TAM_LONG_CLAVE);
	byteString.insertLast(this->key);
	this->key=keyaux;
	return byteString;
}


bool Key::HidratateDecoded(ByteString &byteString, FrontCoder &decoder)
{
	bool response;
	if (byteString.getSize() < TAM_LONG_CLAVE){
		response = false;
	}else{
		this->key = "";
		if(byteString.getSize() > TAM_LONG_CLAVE){
			char tamano;
			byteString.read(&tamano,0,TAM_LONG_CLAVE);
			int size = int(tamano);
			char buffer[size];
			byteString.read(buffer,TAM_LONG_CLAVE,size);
			this->key.append(buffer,size);
			this->key=decoder.decode(this->getKey());
		}
		response = true;
	}
	return response;
}


Key* Key::Clone(){
	return new Key(this->key);
}

int Key::getSize(){
	return (this->key.size());
}

string Key::getKey() const
{
	return this->key;
}

void Key::setKey(string clave)
{
    this->key = clave;
}

bool Key::operator == (const Key& other) const{
	return (this->key == other.key);
}

bool Key::operator != (const Key& other) const{
	return (this->key != other.key);
}

bool Key::operator < (const Key& other) const{
	return (this->key < other.key);
}

bool Key::operator > (const Key& other) const{
	return (this->key > other.key);
}

bool Key::operator <= (const Key& other) const{
	return (this->key <= other.key);
}

