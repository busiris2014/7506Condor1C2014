#include "Reference.h"


Reference::Reference(){
	this->position = -1;//con -1 se indica que no apunta a un bloque
}

Reference::Reference(int pos) {
	this->position = pos;
}

Reference::~Reference() {

}

ByteString Reference::Serialize(){
	ByteString byteString;
	byteString.insertLast(&(this->position),sizeof(int));
	return byteString;
}

bool Reference::Hidratate(ByteString &byteString){
	bool success;
	if ((int)byteString.getSize() != this->getSize()){
		success = false;
	}else{
		this->position = byteString.readAsInt(0);
		success = true;
	}
	return success;
}

int Reference::getPosition() const
{
    return this->position;
}

void Reference::setPosition(int posicion)
{
   this->position = posicion;
}

string Reference::toString(){
	string str;
	str += Utility::intToString(this->position);
	return str;
}

int Reference::getSize(){
	return sizeof(int);
}


