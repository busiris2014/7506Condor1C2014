#ifndef REFERENCE_H_
#define REFERENCE_H_
#include "../common/Key.h"
#include "../common/Serializer.h"
#include <string>
using std::string;

class Reference : public Serializer{

private:
	int position;  /* posicion de memoria en el reference*/

public:
	//Constructor
	Reference();
	Reference(int pos);

	//Destructor
	virtual ~Reference();


	ByteString Serialize();
	bool Hidratate(ByteString &byteString);

	//Getters y setters
    int getPosition() const;
	void setPosition(int position);
	string toString();

	//Metodos estaticos
	static int getSize();
};

#endif /* REFERENCE_H_ */
