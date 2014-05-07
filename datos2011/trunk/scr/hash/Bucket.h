#ifndef BUCKET_H_
#define BUCKET_H_
#include "../common/Record.h"
#include "../common/List.h"
#include "../common/Serializer.h"
using namespace std;

class Bucket : public Serializer{

private:
	List<Record*> RecordList;
	int availableSpace;
	int dispersionNumber;
	/* capacity representa la capacidad del bucket en tamaño de bytes */
	int capacity;
public:
	/*
	 * Constructor
	 * Pre: la capacidad debe ser valida y mayor a 2 enteros (8 bytes)
	 * Pos: queda creada una instancia con la capacidad pasada por parametro,
	 * 	    y el numero de dispersion iniciado en 1.
	 */
	Bucket(int capacity);

	/*
	 * Constructor
	 * Pre: la capacidad debe ser valida y mayor a 2 enteros (8 bytes)
	 * Pos: queda creada una instancia con la capacidad pasada por parametro,
	 * 		y el numero de dispersion iniciado en 1.
	 */
	Bucket(int capacity, int dispersionNumber);

	/*
	 * Metodo que permite buscar un registro dentro de la RecordList
	 * Pre: paso la clave.
	 * Pos: Devuelve el registro si lo encontro; y en caso contrario devuelve NULL.
	 */
	Record* find(Key* key);

	/*
	 * Metodo que sirve para eliminar un registro de la lista de registros.
	 * Pre: paso la clave del registo a eliminar.
	 * Pos: Devuelve TRUE si lo pudo eliminar y FALSE en otros casos.
	 * 		Queda aumentado el espacio disponible.
	 */
	bool deleteRecord (Key* key);

	/*
	 * Metodo que deja al bucket sin contenido.
	 * Pre:-
	 * Pos: El bucket no contiene registros.
	 */
	void toEmpty();
	/*
	 * Metodo que sirve para insertData un registro en la listaRegistro.
	 * Pre: paso la y el registro a insertData.
	 * Pos: Devuelve TRUE si lo pudo insertData y FALSE en otros casos.
	 * 		Queda disminuido el espacio disponible
	 */
	bool insertRecord(Record* data);

	bool isEmpty();
	/*
	 * Getters y Setters.
	 */

	List<Record*>* getRecordList();

	int getCapacity();

    int getAvailableSpace() const;

    int getDispersionNumber() const;

    void setDispersionNumber(int n);

    void setAvailableSpace(int availableSpace);

    ByteString Serialize();

    bool Hidratate(ByteString &byteString);

    string toString(bool isRecordDataInt = false);

    /*
     * Destructor
     */
    virtual ~Bucket();

};

#endif /* BUCKET_H_ */
