#ifndef RECORD_H_
#define RECORD_H_
#include "Key.h"
#include "../common/Serializer.h"
#include "../common/ByteString.h"
#include <string>
using namespace std;
using std::string;

class Record : public Serializer {

private:
	Key* key;
	ByteString* data;

public:
	//Constructor
	Record();
	Record(Key* clave, ByteString* datos);

	//Destructor
	virtual ~Record();

	//Devuelve un nuevo registro copiado del actual
	Record* clone();

	//implementaciones de los metodos virtuales dela clase heredada
	ByteString Serialize();
	bool Hidratate(ByteString &byteString);

	//Getters y setters
    int getSize() const;
	Key* getKey() const;
    static int getMinSize();
    void setKey(Key *key);
    ByteString* getData() const;
    void setData(ByteString* data);

    string toString();
};

#endif /* RECORD_H_ */
