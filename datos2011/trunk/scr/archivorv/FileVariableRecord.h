/*
 * FileVariableRecord.h
 *
 *  Created on: 26/03/2011
 *      Author: luciano
 */

#ifndef FileVariableRecord_H_
#define FileVariableRecord_H_
#include "../common/ByteString.h"
#include <string.h>
#include <fstream>
#include "../common/Record.h"
#include "FileFreeSpace.h"
#define SIZE_LONG_VAR_REC sizeof(int)

/*
 * Clase para manejar los archivos mediante objetos de la clase
 * CadenaBytes.
 * Cada objeto FileVariableRecord se corresponde con un unico file
 */
class FileVariableRecord{

private:
	fstream file;
	string path;
	int fileSize;//indica el tamaño del file, con -1 se indica que no esta abierto
	FileFreeSpace* fileFreeSpace;
	int actualPosition;

	int getBookSize(int position);

public:
	/*
	 * Constructor
	 */
	FileVariableRecord(string path);

	/*
	 * Metodo que sirve para crear un file nuevo.
	 * Pre:
	 * Pos: El file queda creado, pero no abierto. Si lo creo sin problemas devuelve
	 * 		TRUE, de lo contrario, FALSE.
	 */
	bool createFile ();

	/*
	 * Metodo que sirve para abrir el file
	 * Pre: El file debe existir
	 * Pos: Devuelve TRUE si lo pudo abrir bien, de lo contrario FALSE.
	 * 		Queda definido el tamanio del file
	 */
	bool openFile ();

	/*
	 * Metodo que sirve para cerrar el file.
	 * Pre:
	 * Pos: TRUE cerro el file correctamente, de lo contrario FALSE.
	 *
	 * HACEMOS ESTE METODO O DIRECTAMENTE LO CERRAMOS EN EL DESTRUCTOR???
	 */
	void closeFile();

	/*
	 * Metodo que renombra el file con el que se esta trabajando.
	 * Pre:-
	 * Pos: Renombra el file. En caso exitoso retorna 0, y sino
	 * 		retorna el errno.
	 * 		El file queda en el mismo estado en el que estaba antes
	 * 		(abierto o cerrado)
	 */
	int renameFile(string newPath);

	/*
	 * Metodo que remueve el file con el que se esta trabajando.
	 * Pre:-
	 * Pos: Remueve el file. En caso exitoso retorna 0, y sino
	 * 		retorna el errno.
	 * 		El file queda en el mismo estado en el que estaba antes
	 * 		(abierto o cerrado)
	 */
	int deleteFile();

	int getFileSize();

	string getPath();

	/*
	 * Metodo que agrega el byteString al file y devuelve la posicion donde se guardo en el archivo o -1 si no pudo.
	 * Pre: El file debe estar creado y abierto
	 * Pos: Agrega el bytestring en el file en el lugar que encuentre vacio o sino al final.
	 * 		Devuelve la posicion donde se ingreso el registro o -1 si no logro ingresarlo
	 */
	int addBook(ByteString byteString);

	/*
	 * Metodo que libera el espacio ocupado por un registro.
	 * Pre: El file debe estar creado y abierto
	 * Pos: Elimina el registro del file dejando el espacio dispobible para otro ingreso
	 * 		Devuelve el valor true en caso de eliminarse o false en caso de no poder
	 */
	void deleteBook(int position);


	ByteString readBook(int position);

	ByteString readNext();

	/*
	 * Destructor
	 */
	virtual ~FileVariableRecord();

};

#endif /* FileVariableRecord_H_ */
