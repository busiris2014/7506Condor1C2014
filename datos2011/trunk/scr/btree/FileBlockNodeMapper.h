/*
 * FileBlockNodeMapper.h
 *
 *  Created on: 21/04/2011
 *      Author: luciano
 */

#ifndef FILEBLOCKNODEMAPPER_H_
#define FILEBLOCKNODEMAPPER_H_
#define SIZE_BLOCK sizeof(int)
#define SIZE_NODE sizeof(int)
#define NUMBER_NODES sizeof(int)

#include <fstream>
#include "../common/List.h"
#include "../common/ByteString.h"
#include "BlockNodeRecord.h"

class FileBlockNodeMapper {
private:
	List<BlockNodeRecord*> listBlockNodeRecods;
	fstream fileBlockNodeRecords;
	string fileName;
	/*
	 * Metodo que vacia la lista de BlockNodeRecords
	 * Pre: La lista debe estar creada
	 * Pos: Deja la lista vacia
	 */
	void empty();
	/*
	 * Metodo que guarda la lista de nodos en el file
	 * Pre: El file debe estar creado y abierto
	 * Pos:
	 */
	void saveFile();
	/*
	 * Metodo que carga los nodos que estan guardados en el file .dat en la listBlockNodeRecods
	 * Pre: El file debe estar creado y abierto
	 * Pos: Carga los nodos en la lista
	 */
	void loadListBlockNodeRecords();

public:
	/*
	 * Constructor
	 */
	FileBlockNodeMapper(string filename);
	/*
	 * Agrega a la lista de espacios libres el espacio y la posicion pasada
	 * Pre: La lista debe estar creada
	 * Pos: Agrega una instancia de FreeSpace a la listFreeSpaces con los parametros posicion y espacio
	 */
	void addBlockNodeRecord(int node,int block);
	/*
	 * Renombra el file a utilizarse para almacenar los espacios libres
	 * Pre: Ninguna
	 * Pos: Modifica el nombre del file a guardarse
	 */
	void renameFile(string newPath);
	/*
	 *
	 */
	int getBlock(int nodo);
	/*
	 *
	 */
	int getSizeList();
	/*
	 * Destructor
	 */
	virtual ~FileBlockNodeMapper();
};

#endif /* FILEBLOCKNODEMAPPER_H_ */
