#include "TreeConstraits.h"

TreeConstraits::TreeConstraits() {

}

TreeConstraits::~TreeConstraits() {
}

int TreeConstraits::getFullSizeNode(){
	return Resource::getConfigProperties()->getAsInt("booquerio.arbol.nodo.tamanio");
}

int TreeConstraits::getControlSizeNode(){
	return Resource::getConfigProperties()->getAsInt("booquerio.arbol.nodo.control");
}

int TreeConstraits::getEfectiveSizeNode(){
	return Resource::getConfigProperties()->getAsInt("booquerio.arbol.nodo.tamanio") -
			Resource::getConfigProperties()->getAsInt("booquerio.arbol.nodo.control");
}

int TreeConstraits::getControlSizeRecord(){
	return Resource::getConfigProperties()->getAsInt("booquerio.arbol.registro.control");
}

int TreeConstraits::getPercentRecord(){
	return Resource::getConfigProperties()->getAsInt("booquerio.arbol.registro.porcentaje");
}

