
#include "BPlusTreeIterator.h"


BPlusTreeIterator::BPlusTreeIterator() {
	this->nodoActual = NULL;
	this->persistor = NULL;
	this->posicionActual = 0;
}

BPlusTreeIterator::BPlusTreeIterator(LeafNode *hojaCorriente, int posicionActual, FileBlockManager* persistor) {
	this->nodoActual = hojaCorriente;
	this->persistor = persistor;
	this->posicionActual = posicionActual;
}

BPlusTreeIterator::~BPlusTreeIterator() {
	if (nodoActual)
		delete nodoActual;
}

bool BPlusTreeIterator::hasNext() {
	if (nodoActual) {
		if(posicionActual < nodoActual->keyMount)
			return true;
		else
			if (nodoActual->nextLeaf != 0)
				return true;
			else {
				posicionActual = 0;
				delete nodoActual;
				nodoActual = NULL;
				return false;
			}
	}
	return false;
}

Record* BPlusTreeIterator::next() {
	if (!hasNext())
		return NULL;

	if (posicionActual == nodoActual->keyMount){
		LeafNode *nuevoNodoHoja = new LeafNode();
		ByteString cadena = persistor->readBlock(nodoActual->nextLeaf);
		nuevoNodoHoja->Hidratate(cadena);
		nuevoNodoHoja->number = nodoActual->nextLeaf;
		if (nodoActual)
			delete nodoActual;
		nodoActual = nuevoNodoHoja;
		posicionActual = 0;
	}
	Record* registro = new Record(nodoActual->keys[posicionActual].Clone(), new ByteString(nodoActual->byteData[posicionActual]));
	posicionActual++;
	return registro;
}
