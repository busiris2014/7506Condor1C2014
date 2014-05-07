#include "Resource.h"

// Inicializacion del miembro estatico
Properties Resource::configProperties("booquerio.config");

Resource::Resource() {
}

Resource::~Resource() {
}

Properties* Resource::getConfigProperties(){
	Resource::configProperties.setFileName("booquerio.config");
	return &configProperties;
}

