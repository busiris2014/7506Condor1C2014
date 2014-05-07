
#include "Record.h"
#include "../Utility/Utility.h"
#include "Constantes.h"

Record::Record()
{
	this->key = new Key("");
	this->data = new ByteString;
}

Record::Record(Key* key, ByteString* data)
{
	this->key = key;
	this->data = data;
}

Record::~Record() {

	if (this->key) delete (this->key);
	if (this->data) delete (this->data);
}

ByteString Record::Serialize()
{
	ByteString byteString;
	int size = getSize();
	byteString.insertLast(&size,sizeof(int));
	byteString.insertLast(key->Serialize());
	byteString.insertLast(*data);
	return byteString;
}

bool Record::Hidratate(ByteString &byteString){
	bool response;
	//el tamaño debe ser al menos el tamaño de la longitud del registro y la key
	if (byteString.getSize() < (TAM_LONG_REGISTRO + TAM_LONG_CLAVE))  {
		response = false;
	}else{
		if (this->data) {
			delete (this->data);
			this->data = new ByteString;
		}
		int regSize = byteString.readAsInt(0);
		char keySize;
		byteString.read(&keySize,TAM_LONG_REGISTRO,TAM_LONG_CLAVE);
		ByteString stringKey = byteString.read(TAM_LONG_REGISTRO,TAM_LONG_CLAVE + int(keySize));
		this->key->Hidratate(stringKey);
		unsigned int positionData = (TAM_LONG_REGISTRO + TAM_LONG_CLAVE + int(keySize));
		this->data->insertLast(byteString.read(positionData,regSize - positionData));
		response = true;
	}
	return response;
}

Record* Record::clone()
{
	return new Record(key->Clone(),new ByteString(*data));
}

Key *Record::getKey() const
{
    return key;
}

ByteString* Record::getData() const
{
    return data;
}

int Record::getSize() const
{
    return (TAM_LONG_CLAVE + TAM_LONG_REGISTRO + key->getSize() + data->getSize());
}

int Record::getMinSize()
{
    return (TAM_LONG_CLAVE + TAM_LONG_REGISTRO);
}

void Record::setKey(Key *key)
{
	if(this->key) delete this->key;
    this->key = key;
}

void Record::setData(ByteString* data)
{
	if (this->data) delete (this->data);
    this->data = data;
}

string Record::toString(){
	string cadena;
	cadena += key->toString();
	cadena += " | ";
	cadena += data->toString();
	return cadena;
}

