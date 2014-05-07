#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include "ByteString.h"

class Serializer{

public:
	//Metodo a implementar que tiene la logica de serializacion de cada clase
	virtual ByteString Serialize()=0;

	//Metodo a implementar que tiene la logica de hidratacion de cada clase
	virtual bool Hidratate(ByteString &cadena)=0;
};

#endif /* SERIALIZER_H_ */
