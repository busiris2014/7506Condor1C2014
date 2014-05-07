#ifndef BYTESTRING_H_
#define BYTESTRING_H_

#include <string>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include "Constantes.h"

using namespace std;

class ByteString {
private:
	string bytes;

public:
	//Constructores
	ByteString();

	ByteString(const ByteString &byteString);

	ByteString (string bytes);

	ByteString(void* value, int size);

	//Destructor
	virtual ~ByteString();

	//Devuelve true si esta vacio
	bool isEmpty() const;

	//Devuelve el string contenido
	string toString() const;

	//Inserta el string pasado por parametro en el string contenido
	//en la posicion especificada
	bool insert(const ByteString& bytes, unsigned int position);
	bool insert(const string& bytes, unsigned int position);
	bool insert(void* value, unsigned int size, unsigned int position);

	//Inserta el string pasado en el string contenido
	//en el final de la cadena
	bool insertLast(const ByteString& bytes);
	bool insertLast(const string& bytes);
	bool insertLast(void* value, int size);

	//Reemplaza el contenido del string contenido, por el contenido del
	//string pasado por parametro
	bool replace(const string &bytes, unsigned int position);
	bool replace(const ByteString &byteString, unsigned int position);

	//remueve los bytes especificados del string contenido
	bool remove(unsigned int start, unsigned int end);

	//Lee los bytes especificados y los guarda en el buffer
	bool read(char* buffer, unsigned int start, unsigned int size)const;

	//Devuelve los bytes especificados
	ByteString read(unsigned int start, unsigned int size) const;

	//Lee los 4 bytes a partir de la posicion especificada y lo devuelve
	//como un entero
	int readAsInt(unsigned int start);

	//Limpia la cadena contenida
	bool clean();

	//Devuelve el listSize de la cadena contenida
	unsigned int getSize() const;

	//Setea el contenido
	void setByteString(const string &bytes);

	bool fillWithEmpty(unsigned int finalSize);

};

#endif /* BYTESTRING_H_ */
