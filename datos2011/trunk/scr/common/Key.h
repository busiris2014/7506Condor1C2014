
#ifndef KEY_H_
#define KEY_H_
#include "../common/Serializer.h"
#include "../common/ByteString.h"
#include "../Utility/Utility.h"
#include <string.h>
#include "../btree/FrontCoder.h"

class Key : public Serializer{

private:
	string key;

public:

	//Constructor
	Key();
	Key(string clave);

	//Destructor
	virtual ~Key();

	//Devuelve una representacion del estado actual de la key
	string toString() const;

	//Devuelve la key serializada
	ByteString Serialize();

	//Carga el objeto a partir de uno serializado
	bool Hidratate(ByteString &cadena);

	//Devuelve la key serializada y codificada con front coding
	ByteString SerializeEncoded(FrontCoder &encoder);

	//Carga el objeto a partir de uno serializado y codificado con front coding
	bool HidratateDecoded(ByteString &cadena, FrontCoder &decoder);

	//Devuelve otro objeto, con las misma configuracion que el actual
	Key* Clone();

	//Devuelve el listSize de la key
	int getSize();
	//Devuelve el contenido de la key
	string getKey() const;
	//Setea el contenido de la key
	void setKey(string clave);

	//Sobrecarga de operadores para una key
	bool operator != (const Key& other) const;
	bool operator == (const Key& other) const;
	bool operator < (const Key& other) const;
	bool operator > (const Key& other) const;
	bool operator <= (const Key& other) const;
};

#endif /* KEY_H_ */
