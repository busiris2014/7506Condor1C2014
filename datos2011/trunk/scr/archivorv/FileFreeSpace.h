/*
 * FileFreeSpace.h
 *
 *  Created on: 30/03/2011
 *      Author: luciano
 */

#ifndef FILEFREESPACE_H_
#define FILEFREESPACE_H_
#define SIZE_REC_SPACE sizeof(int)
#define SIZE_REC_POS sizeof(int)
#define NUMBER_SPACE sizeof(int)

#include <fstream>
#include "../common/List.h"
#include "../common/ByteString.h"
#include "../archivorv/FreeSpace.h"
#include "../Utility/Utility.h"

class FileFreeSpace {
private:
	List<FreeSpace*> listFreeSpaces;
	fstream fileFreeSpaces;
	string fileName;
	int recordPosition;
	/*
	 * Metodo que vacia la lista de "FreeSpace"
	 * Pre: La lista debe estar creada
	 * Pos: Deja la lista vacia
	 */
	void empty();
	/*
	 * Metodo que guarda la lista de espacios vacias en el file
	 * Pre: El file debe estar creado y abierto
	 * Pos: Agrega el registro en el file en el lugar que encuentre vacio o sino al final.
	 * 		Devuelve la posicion donde se ingreso el registro o -1 si no logro ingresarlo
	 */
	void saveFile();
	/*
	 * Metodo que carga los espacios que estan guardados en el file .dat en la listFreeSpaces
	 * Pre: El file debe estar creado y abierto
	 * Pos: Carga los espacios en la lista
	 */
	void loadListFreeSpaces();
	/*
	 * Metodo que devuelve la posicion del espacio que menos espacio desperiria, o -1 si no hay lugar
	 * Pre: La lista debe estar creada
	 * Pos: Devuelve la posicion donde se puede guardar el registro o -1 si no hay lugar
	 */
	int searchBestSpace(int spaceNeeded);

public:
	/*
	 * Constructor
	 */
	FileFreeSpace(string filename);
	/*
	 * Agrega a la lista de espacios libres el espacio y la posicion pasada
	 * Pre: La lista debe estar creada
	 * Pos: Agrega una instancia de FreeSpace a la listFreeSpaces con los parametros posicion y espacio
	 */
	void addFreeSpace(int position,int space);
	/*
	 * Devuelve puntero a FreeSpace creado con la posicion y espacio para agregar el nuevo registro
	 * Pre: La lista debe estar creada
	 * Pos: Quita el espacio de la listFreeSpaces
	 */
	FreeSpace* getSpace(int spaceNeeded);
	/*
	 * Renombra el file a utilizarse para almacenar los espacios libres
	 * Pre: Ninguna
	 * Pos: Modifica el nombre del file a guardarse
	 */
	void renameFile(string newPath);

	int searchSpace(int position);
	/*
	 * Destructor
	 */
	virtual ~FileFreeSpace();
};

#endif /* FILEFREESPACE_H_ */
