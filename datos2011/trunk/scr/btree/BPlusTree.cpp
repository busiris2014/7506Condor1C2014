
#include "BPlusTree.h"
#include <iostream>


BPlusTree::BPlusTree()
{

}

BPlusTree::BPlusTree(string filePath, int maxKeySize) {
	this->firstLeaf = 0;
	this->keyTopSize = maxKeySize;

	this->fileBlockManager = new FileBlockManager(filePath + ".tree", TreeConstraits::getFullSizeNode());
	if(!this->fileBlockManager->fileOpen()){
		this->fileBlockManager->fileNew();
		this->fileBlockManager->fileOpen();
	}

	this->root = hidratateNode(0);
	if (this->root) {
		this->NodesMount = fileBlockManager->getFileSize() / TreeConstraits::getFullSizeNode();
	} else {
		this->NodesMount = 0;
	}
	hidratateDataConfig();
}


BPlusTree::~BPlusTree() {
	if (root) {
		freeNodeMemory(root);
	}
	delete this->fileBlockManager;
}


bool BPlusTree::add(Record* record) {

	/* Si el tamanio del registro que quiero insertar es mayor al porcentaje TreeConstraits::getPercentRecord() del nodo, lanzo una excepcion */
	if ((int)record->getSize() > (TreeConstraits::getEfectiveSizeNode() * TreeConstraits::getPercentRecord() / 100) || record->getKey()->getSize() > keyTopSize)
		throw new exception;

	Node* newChildNode = NULL;
	Key toPromoteKey;
	if (root == NULL) {
		root = createLeafNode();
		root->number = firstLeaf = 0;
		serializeDataConfig();
		(static_cast <LeafNode*> (root))->nextLeaf = 0;
		this->NodesMount = 1;
	}

	bool result = recursiveInsert(root, *(record->getKey()), *(record->getData()), &toPromoteKey, &newChildNode);

	if (newChildNode) {
		persistNode(newChildNode);
		InnerNode* newRoot = createInnerNode(root->level + 1);

		// Muevo la raiz a otra posicion y persisto la nueva raiz en la posicion cero
		root->number = getNodeNumberToSeize();
		if (root->isLeaf()) {
			firstLeaf = root->number;
			serializeDataConfig();
		}
		persistNode(root);

		newRoot->keys[0] = toPromoteKey;
		newRoot->sons[0] = root->number;
		newRoot->sons[1] = newChildNode->number;
		newRoot->keyMount = 1;
		newRoot->occupiedSpace += toPromoteKey.getSize() + TreeConstraits::getControlSizeRecord();
		newRoot->number = 0;
		persistNode(newRoot);

		freeNodeMemory(root);
		freeNodeMemory(newChildNode);
		root = newRoot;
	} else {
		persistNode(root);
	}
	return result;
}


BPlusTreeIterator* BPlusTree::begin() {
	BPlusTreeIterator* iterator = NULL;
	LeafNode* leaf = static_cast <LeafNode*> (hidratateNode(firstLeaf));
	if (leaf) {
		iterator = new BPlusTreeIterator(leaf->clone(), 0, fileBlockManager);
		freeNodeMemory(leaf);
	}
	return iterator;
}


pair<Record*, BPlusTreeIterator*> BPlusTree::search(Key k) {
	Node *aNode = root;
	if (!aNode)
		return pair<Record*, BPlusTreeIterator*> (NULL, NULL);

	while (!aNode->isLeaf()) {
		InnerNode *innerNode = static_cast<InnerNode*> (aNode);
		int position = getPosition(innerNode, k);
		aNode = hidratateNode(innerNode->sons[position]);
		if (innerNode != root)
			freeNodeMemory(innerNode);
	}
	LeafNode *leafNode = static_cast<LeafNode*> (aNode);
	Record* record = NULL;
	BPlusTreeIterator* iterator = NULL;
	int pos = getPosition(leafNode, k);
	if (pos < leafNode->keyMount && equalKey(k, leafNode->keys[pos])) {
		record = new Record(leafNode->keys[pos].Clone(), new ByteString(leafNode->byteData[pos]));
		iterator = new BPlusTreeIterator(leafNode->clone(), pos, fileBlockManager);
	}
	if (leafNode != root)
		freeNodeMemory(leafNode);
	return pair<Record*, BPlusTreeIterator*> (record, iterator);
}


bool BPlusTree::removeKey(Key k) {
	if (!root)
		return false;
	Result result = BPlusTree::recursiveRemove(k, root, NULL, NULL, NULL, NULL, NULL, 0);
	return !result.contains(Result::NO_ENCONTRADO);
}

void BPlusTree::exportToPlainTextNoOrder(string filePath){

	ofstream salida(filePath.c_str(),ios::out | ios::trunc);
	int j = this->fileBlockManager->getBlockCount();
	for(int i = 0; i < j; i++){
		Node* n = NULL;
		n = this->hidratateNode(i);
		if(n->isLeaf()){
			salida << i << " LEAF ";
			for(int k = 0; k < n->keyMount; k++ )
				salida << " (" << n->keys[k].getKey() << ") ";
		}else{
			salida << i << " INNER ";
			for(int k = 0; k < n->keyMount; k++){
				InnerNode* inner = static_cast<InnerNode*> (n);
				salida << " (" << inner->keys[k].getKey() << ") " << inner->sons[k];

			}
		}
	salida << endl;
	}

}


void BPlusTree::exportToPlainText(string filePath, bool keytext, bool textContent){ // serveto's way

	ofstream salida(filePath.c_str(),ios::out | ios::trunc);

	salida << "--------- Arbol B+ ---------" << endl << endl;

	salida << "Tamanio Total Nodo:  " << TreeConstraits::getFullSizeNode() << endl;
	salida << "Tamanio Datos Control Nodo:  " << TreeConstraits::getControlSizeNode() << endl;
	salida << "Tamanio Efectivo Nodo:  " << TreeConstraits::getEfectiveSizeNode() << endl;
	salida << "Tamanio Datos Control Registro:  " << TreeConstraits::getControlSizeRecord() << endl << endl;
	salida << "Primera Hoja:  " << firstLeaf << endl;
	salida << "Nodos Libres:  ";

	vector<int>::iterator it = freeNodes.begin();
	while (it != freeNodes.end()) {
		int numeroDeNodo = *it;
		salida << numeroDeNodo << " | ";
		it++;
	}
	salida << endl;

	if (root)
		exportNode(salida, root, 1,keytext,textContent);

	salida.close();
}


int BPlusTree::blocksCount(){
	return this->fileBlockManager->getBlockCount();
}


Node* BPlusTree::readNode(int numeroDeNodo) {
	return hidratateNode(numeroDeNodo);
}


/***************** Metodos Privados **********************/


bool BPlusTree::minorKey(Key clave1, Key clave2) {
	return (KeyComparator::compare(clave1, clave2) == -1);
}


bool BPlusTree::minorOrEqualKey(Key clave1, Key clave2) {
	return (KeyComparator::compare(clave1, clave2) != 1);
}


bool BPlusTree::equalKey(Key clave1, Key clave2) {
	return (KeyComparator::compare(clave1, clave2) == 0);
}


LeafNode* BPlusTree::createLeafNode() {
	LeafNode* unNodoHoja = new LeafNode();
	return unNodoHoja;
}


InnerNode* BPlusTree::createInnerNode(int nivel) {
	InnerNode* unNodoInterior = new InnerNode(nivel);
	return unNodoInterior;
}


void BPlusTree::freeNodeMemory(Node *unNodo) {
	if (unNodo->isLeaf()) {
		delete static_cast<LeafNode*> (unNodo);
	} else {
		delete static_cast<InnerNode*> (unNodo);
	}
	unNodo = NULL;
}


int BPlusTree::getPosition(Node *unNodo, Key clave) {

	if (unNodo->keyMount == 0)
		return 0;

	int inferior = 0;
	int superior = (unNodo->keyMount)-1;

	while (inferior < superior) {
		int medio = (inferior + superior) / 2;

		if (minorOrEqualKey(clave, unNodo->keys[medio])) {
			superior = medio - 1;
		} else {
			inferior = medio + 1;
		}
	}

	if (superior < 0 || minorKey(unNodo->keys[superior], clave))
		superior++;

	return superior;
}


bool BPlusTree::recursiveInsert(Node* nodoCorriente, Key clave, ByteString dato, Key* clavePromocion, Node** nuevoNodo) {

	if (!nodoCorriente->isLeaf()) {

		InnerNode *nodoInteriorCorriente = static_cast<InnerNode*> (nodoCorriente);
		Key nuevaClave;
		Node* nuevoNodoHijo = NULL;
		int posicion = getPosition(nodoInteriorCorriente, clave);
		Node* nodoHijo = hidratateNode(nodoInteriorCorriente->sons[posicion]);

		bool resultado = recursiveInsert(nodoHijo, clave, dato, &nuevaClave, &nuevoNodoHijo);

		if (nuevoNodoHijo) {

			if (nodoInteriorCorriente->isOverflow(nuevaClave.getSize() + TreeConstraits::getControlSizeRecord() + keyTopSize)) {

				splitInnerNode(nodoInteriorCorriente, clavePromocion, nuevoNodo, posicion);

				if (posicion == nodoInteriorCorriente->keyMount + 1
						&& nodoInteriorCorriente->keyMount < (*nuevoNodo)->keyMount) {

					InnerNode *nuevoNodoInterior = static_cast<InnerNode*> (*nuevoNodo);
					nodoInteriorCorriente->keys[nodoInteriorCorriente->keyMount] = *clavePromocion;
					nodoInteriorCorriente->sons[nodoInteriorCorriente->keyMount + 1] = nuevoNodoInterior->sons[0];
					nodoInteriorCorriente->keyMount++;
					nodoInteriorCorriente->occupiedSpace += (*clavePromocion).getSize() + TreeConstraits::getControlSizeRecord();
					nuevoNodoInterior->sons[0] = nuevoNodoHijo->number;
					*clavePromocion = nuevaClave;

					persistNode(nuevoNodoHijo);
					freeNodeMemory(nuevoNodoHijo);

					persistNode(nodoHijo);
					freeNodeMemory(nodoHijo);

					return resultado;

				} else {

					if (posicion >= nodoInteriorCorriente->keyMount + 1) {
						posicion -= (nodoInteriorCorriente->keyMount + 1);
						nodoInteriorCorriente = static_cast<InnerNode*> (*nuevoNodo);
					}
				}
			}

			int i = nodoInteriorCorriente->keyMount;
			while (i > posicion) {
				nodoInteriorCorriente->keys[i] = nodoInteriorCorriente->keys[i - 1];
				nodoInteriorCorriente->sons[i + 1] = nodoInteriorCorriente->sons[i];
				i--;
			}
			nodoInteriorCorriente->keys[posicion] = nuevaClave;
			nodoInteriorCorriente->sons[posicion + 1] = nuevoNodoHijo->number;
			nodoInteriorCorriente->keyMount++;
			nodoInteriorCorriente->occupiedSpace += nuevaClave.getSize() + TreeConstraits::getControlSizeRecord();

			persistNode(nuevoNodoHijo);
			freeNodeMemory(nuevoNodoHijo);
		}
		persistNode(nodoHijo);
		freeNodeMemory(nodoHijo);

		return resultado;

	} else {

		LeafNode *nodoHojaCorriente = static_cast<LeafNode*> (nodoCorriente);
		int posicion = getPosition(nodoHojaCorriente, clave);

		// chequea que no exista la clave
		if (posicion < nodoHojaCorriente->keyMount && equalKey(clave, nodoHojaCorriente->keys[posicion])) {
			return false;
		}

		int i = nodoHojaCorriente->keyMount - 1;
		while (i >= 0 && minorKey(clave, nodoHojaCorriente->keys[i])) {
			nodoHojaCorriente->keys[i + 1] = nodoHojaCorriente->keys[i];
			nodoHojaCorriente->byteData[i + 1] = nodoHojaCorriente->byteData[i];
			i--;
		}
		nodoHojaCorriente->keys[i + 1] = clave;
		nodoHojaCorriente->byteData[i + 1] = dato;
		nodoHojaCorriente->keyMount++;
		nodoHojaCorriente->occupiedSpace += dato.getSize() + clave.getSize() + TreeConstraits::getControlSizeRecord();

		if (nodoHojaCorriente->isOverflow(keyTopSize)) {

			splitLeafNode(nodoHojaCorriente, clavePromocion, nuevoNodo);

			if (posicion >= nodoHojaCorriente->keyMount) {
				posicion -= nodoHojaCorriente->keyMount;
				nodoHojaCorriente = static_cast<LeafNode*> (*nuevoNodo);
			}
		}

		if (nuevoNodo && nodoHojaCorriente != *nuevoNodo && posicion == nodoHojaCorriente->keyMount - 1) {
			*clavePromocion = clave;
		}

		return true;
	}
}


void BPlusTree::splitLeafNode(LeafNode* unNodoHoja, Key* clavePromocion, Node** nuevoNodoHoja) {

	int espacioMedio = (unNodoHoja->occupiedSpace) / 2;
	int espacioNodoIzquierdo = 0;
	int cantidadClaves = 0;
	while (cantidadClaves < unNodoHoja->keyMount && espacioNodoIzquierdo < espacioMedio){
		espacioNodoIzquierdo += unNodoHoja->byteData[cantidadClaves].getSize() + unNodoHoja->keys[cantidadClaves].getSize() + TreeConstraits::getControlSizeRecord();
		cantidadClaves++;
		if (espacioNodoIzquierdo > TreeConstraits::getEfectiveSizeNode()) {
			cantidadClaves--;
			espacioNodoIzquierdo -= (unNodoHoja->byteData[cantidadClaves].getSize() + unNodoHoja->keys[cantidadClaves].getSize() + TreeConstraits::getControlSizeRecord());
			break;
		}
	}

	LeafNode *auxNuevoNodoHoja = createLeafNode();
	auxNuevoNodoHoja->number = getNodeNumberToSeize();
	auxNuevoNodoHoja->keyMount = unNodoHoja->keyMount - cantidadClaves;
	auxNuevoNodoHoja->occupiedSpace = unNodoHoja->occupiedSpace - espacioNodoIzquierdo;
	auxNuevoNodoHoja->nextLeaf = unNodoHoja->nextLeaf;

	for (int posicion = cantidadClaves; posicion < unNodoHoja->keyMount; ++posicion) {
		int auxPosicion = posicion - cantidadClaves;
		auxNuevoNodoHoja->keys[auxPosicion] = unNodoHoja->keys[posicion];
		auxNuevoNodoHoja->byteData[auxPosicion] = unNodoHoja->byteData[posicion];
	}

	unNodoHoja->occupiedSpace -= auxNuevoNodoHoja->occupiedSpace;
	unNodoHoja->keyMount = cantidadClaves;
	unNodoHoja->nextLeaf = auxNuevoNodoHoja->number;
	*clavePromocion = unNodoHoja->keys[unNodoHoja->keyMount - 1];
	*nuevoNodoHoja = auxNuevoNodoHoja;
}


void BPlusTree::splitInnerNode(InnerNode* unNodoInterior, Key* clavePromocion, Node** nuevoNodoInterior, int nuevaPosicion) {

	int medio = unNodoInterior->keyMount / 2;

	if (nuevaPosicion <= medio && medio > unNodoInterior->keyMount - (medio + 1)) {
		medio--;
	}

	InnerNode *auxNuevoNodoInterior = createInnerNode(unNodoInterior->level);
	auxNuevoNodoInterior->number = getNodeNumberToSeize();
	auxNuevoNodoInterior->keyMount = unNodoInterior->keyMount - (medio + 1);
	for (int posicion = medio + 1; posicion < unNodoInterior->keyMount; ++posicion) {
		int auxPosicion = posicion - (medio + 1);
		auxNuevoNodoInterior->keys[auxPosicion] = unNodoInterior->keys[posicion];
		auxNuevoNodoInterior->sons[auxPosicion] = unNodoInterior->sons[posicion];
		auxNuevoNodoInterior->occupiedSpace += unNodoInterior->keys[posicion].getSize() + TreeConstraits::getControlSizeRecord();
	}

	auxNuevoNodoInterior->sons[auxNuevoNodoInterior->keyMount] = unNodoInterior->sons[unNodoInterior->keyMount];
	unNodoInterior->keyMount = medio;
	unNodoInterior->occupiedSpace -= auxNuevoNodoInterior->occupiedSpace;
	*clavePromocion = unNodoInterior->keys[medio];
	*nuevoNodoInterior = auxNuevoNodoInterior;
}


Result BPlusTree::recursiveRemove(Key clave, Node *nodoCorriente, Node *nodoIzquierda, Node *nodoDerecha,
		InnerNode *nodoPadreIzquierda, InnerNode *nodoPadreDerecha, InnerNode *nodoPadre, int posicionPadre) {

	if (nodoCorriente->isLeaf()) {

		LeafNode *nodoHojaCorriente = static_cast<LeafNode*> (nodoCorriente);
		LeafNode *nodoHojaIzquierda = static_cast<LeafNode*> (nodoIzquierda);
		LeafNode *nodoHojaDerecha = static_cast<LeafNode*> (nodoDerecha);
		int posicion = getPosition(nodoHojaCorriente, clave);
		if (posicion >= nodoHojaCorriente->keyMount || !equalKey(clave, nodoHojaCorriente->keys[posicion])) {
			return Result::NO_ENCONTRADO;
		}

		nodoHojaCorriente->occupiedSpace -= (nodoHojaCorriente->byteData[posicion].getSize() + nodoHojaCorriente->keys[posicion].getSize() + TreeConstraits::getControlSizeRecord());
		nodoHojaCorriente->keyMount--;
		for (int i = posicion; i < nodoHojaCorriente->keyMount; i++) {
			nodoHojaCorriente->keys[i] = nodoHojaCorriente->keys[i + 1];
			nodoHojaCorriente->byteData[i] = nodoHojaCorriente->byteData[i + 1];
		}

		Result resultado = Result::OK;

		// si se borro el elemento de la ultima posicion y no es la raiz
		if (posicion == nodoHojaCorriente->keyMount && nodoPadre) {
			if (posicionPadre < nodoPadre->keyMount) {
				if (nodoHojaCorriente->keyMount >= 1) {
					nodoPadre->occupiedSpace -= nodoPadre->keys[posicionPadre].getSize();
					nodoPadre->occupiedSpace += nodoHojaCorriente->keys[nodoHojaCorriente->keyMount - 1].getSize();
					nodoPadre->keys[posicionPadre] = nodoHojaCorriente->keys[nodoHojaCorriente->keyMount - 1];
				}
			} else {
				if (nodoHojaCorriente->keyMount >= 1) {
					resultado |= Result (Result::ACTUALIZAR_ULTIMA_CLAVE, nodoHojaCorriente->keys[nodoHojaCorriente->keyMount - 1]);
				} else {
					resultado |= Result (Result::ACTUALIZAR_ULTIMA_CLAVE, nodoHojaIzquierda->keys[nodoHojaIzquierda->keyMount - 1]);
				}
			}
		}

		if (nodoHojaCorriente->isUnderflow() && !(nodoHojaCorriente == root && nodoHojaCorriente->keyMount >= 1)) {

			if (nodoHojaIzquierda == NULL && nodoHojaDerecha == NULL) {
				persistNode(root);
				if (root)
					freeNodeMemory(root);
				root = nodoHojaCorriente = NULL;
				firstLeaf = 0;
				string archivoConfiguracion = fileBlockManager->getPath() + ".cnf";
				remove(archivoConfiguracion.c_str());
				return Result::OK;
			// @fusion
			} else if (( (nodoHojaIzquierda == NULL || !nodoHojaIzquierda->elementsCanTransfer())
						&& (nodoHojaDerecha == NULL	|| !nodoHojaDerecha->elementsCanTransfer()))
						|| nodoHojaCorriente->keyMount == 0) {

					if (nodoPadreIzquierda == nodoPadre) { //  cte e izq son hermanos.
						resultado |= leafNodeFusion(nodoHojaIzquierda, nodoHojaCorriente);
					}else {
						resultado |= leafNodeFusion(nodoHojaCorriente, nodoHojaDerecha);
					}
			// si la derecha mas cargada redistribuyo
			} else if ((nodoHojaIzquierda != NULL && !nodoHojaIzquierda->elementsCanTransfer())
						&& (nodoHojaDerecha != NULL	&& nodoHojaDerecha->elementsCanTransfer())) {

					if (nodoPadreDerecha == nodoPadre) {
						resultado |= redistributeLeftLeaf(nodoHojaCorriente, nodoHojaDerecha, nodoPadreDerecha, posicionPadre);
					} else {
						resultado |= leafNodeFusion(nodoHojaIzquierda, nodoHojaCorriente);
					}
			// si la izquierda mas cargada  redistribuyo
			} else	if ((nodoHojaIzquierda != NULL && nodoHojaIzquierda->elementsCanTransfer())
						&& (nodoHojaDerecha != NULL && !nodoHojaDerecha->elementsCanTransfer())) {

					if (nodoPadreIzquierda == nodoPadre) {
						redistributeRightLeaf(nodoHojaIzquierda, nodoHojaCorriente,	nodoPadreIzquierda, posicionPadre - 1);
					} else {
						resultado |= leafNodeFusion(nodoHojaCorriente, nodoHojaDerecha);
					}
			// izq cte y der son todos hermanos, me fijo cual tiene mas carga y redistribuyo
			} else	if (nodoPadreIzquierda == nodoPadreDerecha) {
					if (nodoHojaIzquierda->occupiedSpace <= nodoHojaDerecha->occupiedSpace) {
						resultado |= redistributeLeftLeaf(nodoHojaCorriente, nodoHojaDerecha, nodoPadreDerecha, posicionPadre);
					} else {
						redistributeRightLeaf(nodoHojaIzquierda, nodoHojaCorriente, nodoPadreIzquierda, posicionPadre - 1);
					}

			} else {
				if (nodoPadreIzquierda == nodoPadre) {
					redistributeRightLeaf(nodoHojaIzquierda, nodoHojaCorriente,	nodoPadreIzquierda, posicionPadre - 1);
				} else {
					resultado |= redistributeLeftLeaf(nodoHojaCorriente, nodoHojaDerecha, nodoPadreDerecha, posicionPadre);
				}
			}
		} else {
			persistNode(nodoHojaCorriente);
		}
		return resultado;

	} else {

		InnerNode *nodoInteriorCorriente = static_cast<InnerNode*> (nodoCorriente);
		InnerNode *nodoInteriorIzquierda = static_cast<InnerNode*> (nodoIzquierda);
		InnerNode *nodoInteriorDerecha = static_cast<InnerNode*> (nodoDerecha);
		Node *auxNodoIzquierda, *auxNodoDerecha;
		InnerNode *auxPadreIzquierda, *auxPadreDerecha;

		int posicion = getPosition(nodoInteriorCorriente, clave);
		if (posicion == 0) {
			auxNodoIzquierda = (nodoIzquierda == NULL) ? NULL : hidratateNode((static_cast<InnerNode*> (nodoIzquierda))->sons[nodoIzquierda->keyMount]);
			auxPadreIzquierda = nodoPadreIzquierda;
		} else {
			auxNodoIzquierda = hidratateNode(nodoInteriorCorriente->sons[posicion - 1]);
			auxPadreIzquierda = nodoInteriorCorriente;
		}

		if (posicion == nodoInteriorCorriente->keyMount) {
			auxNodoDerecha = (nodoDerecha == NULL) ? NULL : hidratateNode((static_cast<InnerNode*> (nodoDerecha))->sons[0]);
			auxPadreDerecha = nodoPadreDerecha;
		} else {
			auxNodoDerecha = hidratateNode(nodoInteriorCorriente->sons[posicion + 1]);
			auxPadreDerecha = nodoInteriorCorriente;
		}
//		if(clave.getKey().compare("438") == 0)
//			std::cout << "llamada recursiva: nodointeriorcorriente: " << nodoInteriorCorriente->number << "posicion: " << posicion << endl;
		Node* auxNodoCorriente = hidratateNode(nodoInteriorCorriente->sons[posicion]);
		Result resultadoParcial = recursiveRemove(clave, auxNodoCorriente, auxNodoIzquierda, auxNodoDerecha, auxPadreIzquierda, auxPadreDerecha, nodoInteriorCorriente, posicion);
		Result resultado = Result::OK;

		if (auxNodoIzquierda)
			freeNodeMemory(auxNodoIzquierda);
		if (auxNodoDerecha)
			freeNodeMemory(auxNodoDerecha);
		if (auxNodoCorriente)
			freeNodeMemory(auxNodoCorriente);

		if (resultadoParcial.contains(Result::NO_ENCONTRADO)) {
			return resultadoParcial;
		}

		if (resultadoParcial.contains(Result::ACTUALIZAR_ULTIMA_CLAVE)) {
			if (nodoPadre && posicionPadre < nodoPadre->keyMount) {
				nodoPadre->occupiedSpace -= nodoPadre->keys[posicionPadre].getSize();
				nodoPadre->occupiedSpace += resultadoParcial.ultimaClave.getSize();
				nodoPadre->keys[posicionPadre] = resultadoParcial.ultimaClave;
			} else {
				resultado |= Result(Result::ACTUALIZAR_ULTIMA_CLAVE, resultadoParcial.ultimaClave);
			}
		}

		if (resultadoParcial.contains(Result::FUSION_NODOS)) {
			Node* nodoHijo = hidratateNode(nodoInteriorCorriente->sons[posicion]);
			if (nodoHijo->keyMount != 0)
				posicion++;

			Key claveInteriorBorrada = nodoInteriorCorriente->keys[posicion - 1];
			for (int i = posicion; i < nodoInteriorCorriente->keyMount; i++) {
				nodoInteriorCorriente->keys[i - 1] = nodoInteriorCorriente->keys[i];
				nodoInteriorCorriente->sons[i] = nodoInteriorCorriente->sons[i + 1];
			}
			nodoInteriorCorriente->keyMount--;
			nodoInteriorCorriente->occupiedSpace -= (claveInteriorBorrada.getSize() + TreeConstraits::getControlSizeRecord());
			nodoInteriorCorriente->occupiedSpace -= nodoInteriorCorriente->keys[nodoInteriorCorriente->keyMount].getSize();

			if (nodoHijo)
				freeNodeMemory(nodoHijo);
			if (nodoInteriorCorriente->level == 1) {
				posicion--;
				nodoHijo = hidratateNode(nodoInteriorCorriente->sons[posicion]);
				nodoInteriorCorriente->occupiedSpace -= nodoInteriorCorriente->keys[posicion].getSize();
				nodoInteriorCorriente->occupiedSpace += nodoHijo->keys[nodoHijo->keyMount - 1].getSize();
				nodoInteriorCorriente->keys[posicion] = nodoHijo->keys[nodoHijo->keyMount - 1];
				if (nodoHijo)
					freeNodeMemory(nodoHijo);
			}
		}

		if (resultadoParcial.contains(Result::FUSION_NODOS)
				&& nodoInteriorCorriente->isUnderflow()
				&& !(nodoInteriorCorriente == root && nodoInteriorCorriente->keyMount >= 1)) {

			if (nodoInteriorIzquierda == NULL && nodoInteriorDerecha == NULL) {
				root = hidratateNode(nodoInteriorCorriente->sons[0]);
				root->number = 0;
				persistNode(root);
				freeNodes.push_back(nodoInteriorCorriente->sons[0]);
				serializeDataConfig();
				return Result::OK;

			} else if ((nodoInteriorIzquierda == NULL || !nodoInteriorIzquierda->elementsCanTransfer())
					&& (nodoInteriorDerecha == NULL || !nodoInteriorDerecha->elementsCanTransfer())) {

				if (nodoPadreIzquierda == nodoPadre) {
					resultado |= innerNodeFusion(nodoInteriorIzquierda, nodoInteriorCorriente, nodoPadreIzquierda, posicionPadre - 1);
				} else {
					resultado |= innerNodeFusion(nodoInteriorCorriente, nodoInteriorDerecha, nodoPadreDerecha, posicionPadre);
				}

			} else if ((nodoInteriorIzquierda != NULL && !nodoInteriorIzquierda->elementsCanTransfer())
					&& (nodoInteriorDerecha != NULL && nodoInteriorDerecha->elementsCanTransfer())) {

				if (nodoPadreDerecha == nodoPadre) {
					redistributeLeftInner(nodoInteriorCorriente, nodoInteriorDerecha, nodoPadreDerecha, posicionPadre);
				} else {
					resultado |= innerNodeFusion(nodoInteriorIzquierda, nodoInteriorCorriente, nodoPadreIzquierda, posicionPadre - 1);
				}

			} else if ((nodoInteriorIzquierda != NULL && nodoInteriorIzquierda->elementsCanTransfer())
					&& (nodoInteriorDerecha != NULL && !nodoInteriorDerecha->elementsCanTransfer())) {

				if (nodoPadreIzquierda == nodoPadre) {
					redistributeRightInner(nodoInteriorIzquierda, nodoInteriorCorriente, nodoPadreIzquierda, posicionPadre - 1);
				} else {
					resultado |= innerNodeFusion(nodoInteriorCorriente, nodoInteriorDerecha, nodoPadreDerecha, posicionPadre);
				}

			} else if (nodoPadreIzquierda == nodoPadreDerecha) {

				if (nodoInteriorIzquierda->keyMount <= nodoInteriorDerecha->keyMount) {
					redistributeLeftInner(nodoInteriorCorriente, nodoInteriorDerecha, nodoPadreDerecha, posicionPadre);
				} else {
					redistributeRightInner(nodoInteriorIzquierda, nodoInteriorCorriente, nodoPadreIzquierda, posicionPadre - 1);
				}

			} else {

				if (nodoPadreIzquierda == nodoPadre) {
					redistributeRightInner(nodoInteriorIzquierda, nodoInteriorCorriente, nodoPadreIzquierda, posicionPadre - 1);
				} else {
					redistributeLeftInner(nodoInteriorCorriente, nodoInteriorDerecha, nodoPadreDerecha, posicionPadre);
				}
			}

		} else {
			persistNode(nodoInteriorCorriente);
		}

		return resultado;
	}
}


Result BPlusTree::leafNodeFusion(LeafNode* hojaIzquierda, LeafNode* hojaDerecha) {

	for (int i = 0; i < hojaDerecha->keyMount; i++) {
		hojaIzquierda->keys[hojaIzquierda->keyMount + i] = hojaDerecha->keys[i];
		hojaIzquierda->byteData[hojaIzquierda->keyMount + i] = hojaDerecha->byteData[i];
	}
	hojaIzquierda->keyMount += hojaDerecha->keyMount;
	hojaIzquierda->occupiedSpace += hojaDerecha->occupiedSpace;
	hojaIzquierda->nextLeaf = hojaDerecha->nextLeaf;

	hojaDerecha->keyMount = 0;
	hojaDerecha->occupiedSpace = 0;

	freeNodes.push_back(hojaDerecha->number);
	serializeDataConfig();

	persistNode(hojaIzquierda);
	persistNode(hojaDerecha);

	return Result::FUSION_NODOS;
}


Result BPlusTree::innerNodeFusion(InnerNode* nodoIzquierda, InnerNode* nodoDerecha, InnerNode* nodoPadre, int posicionPadre) {

	Result resultado;
	int espacioOcupadoTotal = (nodoIzquierda->occupiedSpace + nodoDerecha->occupiedSpace + nodoPadre->keys[posicionPadre].getSize() + TreeConstraits::getControlSizeRecord());

	if (espacioOcupadoTotal > TreeConstraits::getEfectiveSizeNode()) {
		resultado = Result::OK;
	} else {
		nodoIzquierda->keys[nodoIzquierda->keyMount] = nodoPadre->keys[posicionPadre];
		nodoIzquierda->keyMount++;
		nodoIzquierda->occupiedSpace += nodoPadre->keys[posicionPadre].getSize() + TreeConstraits::getControlSizeRecord();
		for (int i = 0; i < nodoDerecha->keyMount; i++) {
			nodoIzquierda->keys[nodoIzquierda->keyMount + i] = nodoDerecha->keys[i];
			nodoIzquierda->sons[nodoIzquierda->keyMount + i] = nodoDerecha->sons[i];
		}
		nodoIzquierda->keyMount += nodoDerecha->keyMount;
		nodoIzquierda->occupiedSpace += nodoDerecha->occupiedSpace;
		nodoIzquierda->sons[nodoIzquierda->keyMount] = nodoDerecha->sons[nodoDerecha->keyMount];
		nodoDerecha->keyMount = 0;
		nodoDerecha->occupiedSpace = 0;

		freeNodes.push_back(nodoDerecha->number);
		serializeDataConfig();

		resultado = Result::FUSION_NODOS;
	}
	persistNode(nodoIzquierda);
	persistNode(nodoDerecha);

	return resultado;
}


Result BPlusTree::redistributeLeftLeaf(LeafNode *hojaIzquierda, LeafNode *hojaDerecha, InnerNode *nodoPadre, int posicionPadre) {

	int tamanioMedio = (hojaDerecha->occupiedSpace - hojaIzquierda->occupiedSpace) / 2;
	int espacioDesplazado = 0;
	int cantidadClavesDesplazadas = 0;
	while (cantidadClavesDesplazadas < hojaDerecha->keyMount && espacioDesplazado < tamanioMedio){
		espacioDesplazado += hojaDerecha->byteData[cantidadClavesDesplazadas].getSize()
						   + hojaDerecha->keys[cantidadClavesDesplazadas].getSize()
						   + TreeConstraits::getControlSizeRecord();
		cantidadClavesDesplazadas++;
		if (espacioDesplazado + hojaIzquierda->occupiedSpace > TreeConstraits::getEfectiveSizeNode()) {
			cantidadClavesDesplazadas--;
			espacioDesplazado -= (hojaDerecha->byteData[cantidadClavesDesplazadas].getSize()
							   + hojaDerecha->keys[cantidadClavesDesplazadas].getSize()
							   + TreeConstraits::getControlSizeRecord());
			break;
		}
	}

	if (cantidadClavesDesplazadas > 0) {
		for (int i = 0; i < cantidadClavesDesplazadas; i++) {
			hojaIzquierda->keys[hojaIzquierda->keyMount + i] = hojaDerecha->keys[i];
			hojaIzquierda->byteData[hojaIzquierda->keyMount + i] = hojaDerecha->byteData[i];
		}

		hojaIzquierda->keyMount += cantidadClavesDesplazadas;
		hojaIzquierda->occupiedSpace += espacioDesplazado;
		hojaDerecha->keyMount -= cantidadClavesDesplazadas;
		hojaDerecha->occupiedSpace -= espacioDesplazado;
		for (int i = 0; i < hojaDerecha->keyMount; i++) {
			hojaDerecha->keys[i] = hojaDerecha->keys[i + cantidadClavesDesplazadas];
			hojaDerecha->byteData[i] = hojaDerecha->byteData[i + cantidadClavesDesplazadas];
		}
	}
	persistNode(hojaIzquierda);
	persistNode(hojaDerecha);

	if (cantidadClavesDesplazadas > 0) {
		if (posicionPadre < nodoPadre->keyMount) {
			nodoPadre->occupiedSpace -= nodoPadre->keys[posicionPadre].getSize();
			nodoPadre->occupiedSpace += hojaIzquierda->keys[hojaIzquierda->keyMount - 1].getSize();
			nodoPadre->keys[posicionPadre] = hojaIzquierda->keys[hojaIzquierda->keyMount - 1];
			return Result::OK;
		} else {
			return Result(Result::ACTUALIZAR_ULTIMA_CLAVE, hojaIzquierda->keys[hojaIzquierda->keyMount - 1]);
		}
	} else {
		return Result::OK;
	}
}


void BPlusTree::redistributeLeftInner(InnerNode *nodoIzquierda, InnerNode *nodoDerecha,	InnerNode *nodoPadre, int posicionPadre) {

	int tamanioMedio = (nodoDerecha->occupiedSpace - nodoIzquierda->occupiedSpace) / 2;
	int espacioDesplazado = 0;
	int cantidadClavesDesplazadas = 0;
	while (cantidadClavesDesplazadas < nodoDerecha->keyMount && espacioDesplazado < tamanioMedio){
		espacioDesplazado += nodoDerecha->keys[cantidadClavesDesplazadas].getSize()
						   + TreeConstraits::getControlSizeRecord();
		cantidadClavesDesplazadas++;
		if (espacioDesplazado + nodoIzquierda->occupiedSpace > TreeConstraits::getEfectiveSizeNode()) {
			cantidadClavesDesplazadas--;
			espacioDesplazado -= (nodoDerecha->keys[cantidadClavesDesplazadas].getSize()
							   + TreeConstraits::getControlSizeRecord());
			break;
		}
	}

	if (cantidadClavesDesplazadas > 0) {
		espacioDesplazado = 0;

		nodoIzquierda->keys[nodoIzquierda->keyMount] = nodoPadre->keys[posicionPadre];
		nodoIzquierda->keyMount++;
		nodoIzquierda->occupiedSpace += nodoPadre->keys[posicionPadre].getSize() + TreeConstraits::getControlSizeRecord();

		for (int i = 0; i < cantidadClavesDesplazadas - 1; i++) {
			nodoIzquierda->keys[nodoIzquierda->keyMount + i] = nodoDerecha->keys[i];
			nodoIzquierda->sons[nodoIzquierda->keyMount + i] = nodoDerecha->sons[i];
			espacioDesplazado += nodoDerecha->keys[i].getSize() + TreeConstraits::getControlSizeRecord();
		}

		nodoIzquierda->keyMount += cantidadClavesDesplazadas - 1;
		nodoIzquierda->occupiedSpace += espacioDesplazado;
		nodoIzquierda->sons[nodoIzquierda->keyMount] = nodoDerecha->sons[cantidadClavesDesplazadas - 1];
		nodoPadre->occupiedSpace -= nodoPadre->keys[posicionPadre].getSize();
		nodoPadre->occupiedSpace += nodoDerecha->keys[cantidadClavesDesplazadas - 1].getSize();
		nodoPadre->keys[posicionPadre] = nodoDerecha->keys[cantidadClavesDesplazadas - 1];
		nodoDerecha->keyMount -= cantidadClavesDesplazadas;
		nodoDerecha->occupiedSpace -= espacioDesplazado;
		nodoDerecha->occupiedSpace -= nodoDerecha->keys[cantidadClavesDesplazadas - 1].getSize();

		for (int i = 0; i < nodoDerecha->keyMount; i++) {
			nodoDerecha->keys[i] = nodoDerecha->keys[i + cantidadClavesDesplazadas];
			nodoDerecha->sons[i] = nodoDerecha->sons[i + cantidadClavesDesplazadas];
		}
		nodoDerecha->sons[nodoDerecha->keyMount] = nodoDerecha->sons[nodoDerecha->keyMount + cantidadClavesDesplazadas];
	}
	persistNode(nodoIzquierda);
	persistNode(nodoDerecha);
}


void BPlusTree::redistributeRightLeaf(LeafNode *hojaIzquierda, LeafNode *hojaDerecha, InnerNode *nodoPadre, int posicionPadre) {

	int espacioOcupadoMedio = (hojaIzquierda->occupiedSpace - hojaDerecha->occupiedSpace) / 2;
	int espacioDesplazado = 0;
	int cantidadClavesDesplazadas = 0;
	for (int i = hojaIzquierda->keyMount; i > 0 && espacioDesplazado < espacioOcupadoMedio; i--) {
		espacioDesplazado += hojaIzquierda->byteData[i-1].getSize() + hojaIzquierda->keys[i-1].getSize() + TreeConstraits::getControlSizeRecord();
		cantidadClavesDesplazadas++;
		if (espacioDesplazado + hojaDerecha->occupiedSpace > TreeConstraits::getEfectiveSizeNode()) {
			espacioDesplazado -= (hojaIzquierda->byteData[i-1].getSize() + hojaIzquierda->keys[i-1].getSize() + TreeConstraits::getControlSizeRecord());
			cantidadClavesDesplazadas--;
			break;
		}
	}

	if (cantidadClavesDesplazadas > 0) {
		for (int i = hojaDerecha->keyMount; i >= 0; i--) {
			hojaDerecha->keys[i + cantidadClavesDesplazadas] = hojaDerecha->keys[i];
			hojaDerecha->byteData[i + cantidadClavesDesplazadas] = hojaDerecha->byteData[i];
		}
		hojaDerecha->keyMount += cantidadClavesDesplazadas;
		hojaDerecha->occupiedSpace += espacioDesplazado;

		for (int i = 0; i < cantidadClavesDesplazadas; i++) {
			hojaDerecha->keys[i] = hojaIzquierda->keys[hojaIzquierda->keyMount - cantidadClavesDesplazadas + i];
			hojaDerecha->byteData[i] = hojaIzquierda->byteData[hojaIzquierda->keyMount - cantidadClavesDesplazadas + i];
		}
		hojaIzquierda->keyMount -= cantidadClavesDesplazadas;
		hojaIzquierda->occupiedSpace -= espacioDesplazado;
		nodoPadre->occupiedSpace -= nodoPadre->keys[posicionPadre].getSize();
		nodoPadre->occupiedSpace += hojaIzquierda->keys[hojaIzquierda->keyMount - 1].getSize();
		nodoPadre->keys[posicionPadre] = hojaIzquierda->keys[hojaIzquierda->keyMount - 1];
	}
	persistNode(hojaIzquierda);
	persistNode(hojaDerecha);
}


void BPlusTree::redistributeRightInner(InnerNode *nodoIzquierda, InnerNode *nodoDerecha, InnerNode *nodoPadre, int posicionPadre) {

	int espacioOcupadoMedio = (nodoIzquierda->occupiedSpace - nodoDerecha->occupiedSpace) / 2;
	int espacioDesplazado = 0;
	int cantidadClavesDesplazadas = 0;
	for (int i = nodoIzquierda->keyMount; i > 0 && espacioDesplazado < espacioOcupadoMedio; i--) {
		espacioDesplazado += nodoIzquierda->keys[i-1].getSize() + TreeConstraits::getControlSizeRecord();
		cantidadClavesDesplazadas++;
		if (espacioDesplazado + nodoDerecha->occupiedSpace > TreeConstraits::getEfectiveSizeNode()) {
			espacioDesplazado -= (nodoIzquierda->keys[i-1].getSize() + TreeConstraits::getControlSizeRecord());
			cantidadClavesDesplazadas--;
			break;
		}
	}

	if (cantidadClavesDesplazadas > 0) {
		espacioDesplazado = 0;

		nodoDerecha->sons[nodoDerecha->keyMount + cantidadClavesDesplazadas] = nodoDerecha->sons[nodoDerecha->keyMount];

		for (int i = nodoDerecha->keyMount - 1; i >= 0; i--) {
			nodoDerecha->keys[i + cantidadClavesDesplazadas] = nodoDerecha->keys[i];
			nodoDerecha->sons[i + cantidadClavesDesplazadas] = nodoDerecha->sons[i];
		}

		nodoDerecha->keyMount += cantidadClavesDesplazadas;
		nodoDerecha->keys[cantidadClavesDesplazadas - 1] = nodoPadre->keys[posicionPadre];
		nodoDerecha->sons[cantidadClavesDesplazadas - 1] = nodoIzquierda->sons[nodoIzquierda->keyMount];
		nodoDerecha->occupiedSpace += nodoPadre->keys[posicionPadre].getSize()+ TreeConstraits::getControlSizeRecord();

		for (int i = 0; i < cantidadClavesDesplazadas - 1; i++) {
			nodoDerecha->keys[i] = nodoIzquierda->keys[nodoIzquierda->keyMount - cantidadClavesDesplazadas + i + 1];
			nodoDerecha->sons[i] = nodoIzquierda->sons[nodoIzquierda->keyMount - cantidadClavesDesplazadas + i + 1];
			espacioDesplazado += nodoDerecha->keys[i].getSize() + TreeConstraits::getControlSizeRecord();
		}
		nodoDerecha->occupiedSpace += espacioDesplazado;

		nodoPadre->occupiedSpace -= nodoPadre->keys[posicionPadre].getSize();
		nodoPadre->occupiedSpace += nodoIzquierda->keys[nodoIzquierda->keyMount - cantidadClavesDesplazadas].getSize();
		nodoPadre->keys[posicionPadre] = nodoIzquierda->keys[nodoIzquierda->keyMount - cantidadClavesDesplazadas];
		nodoIzquierda->keyMount -= cantidadClavesDesplazadas;
		nodoIzquierda->occupiedSpace -= espacioDesplazado;
		nodoIzquierda->occupiedSpace -= (nodoIzquierda->keys[nodoIzquierda->keyMount - cantidadClavesDesplazadas].getSize());
	}
	persistNode(nodoIzquierda);
	persistNode(nodoDerecha);
}


void BPlusTree::exportNode(ofstream& salida, Node* unNodo, int tabulacion,bool keyText,bool textContent) {

	if (unNodo->isLeaf()) {

		LeafNode *unNodoHoja = static_cast<LeafNode*> (unNodo);
		salida << endl;
		for(int i = 0 ; i < tabulacion ; i++)
			salida << "  ";

		salida << "Numero: " << unNodoHoja->number << "  Nivel: " << unNodoHoja->level << "  Cant.Elem: " << unNodoHoja->keyMount
			   << "  Esp.Libre: " << TreeConstraits::getEfectiveSizeNode() - unNodoHoja->occupiedSpace << "  Hoja.Sig: " << unNodoHoja->nextLeaf << endl;

		for (int posicion = 0; posicion < unNodoHoja->keyMount; ++posicion) {

			for(int i = 0 ; i < tabulacion + 4 ; i++)
						salida << "  ";

			salida << "(";

			Key unaClave = unNodoHoja->keys[posicion];
			if(keyText)
			{
				salida << unaClave.toString();
			}
			else
			{
				ByteString clave(unaClave.toString());
				salida << Utility::toString(clave.readAsInt(0));
			}

			salida << ";";
			ByteString unDato = unNodoHoja->byteData[posicion];
			if(textContent)
			{
				salida << unDato.toString();
			}
			else
			{
				salida << Utility::intToString(unDato.readAsInt(0));
			}

			salida << ")";

			salida << (unaClave.getSize() + unDato.getSize() + TreeConstraits::getControlSizeRecord()) << endl;
		}

		salida << endl;

	} else {

		InnerNode *unNodoInterior = static_cast<InnerNode*> (unNodo);
		salida << endl << endl;
		for(int i=0; i<tabulacion ; i++)
			salida << "  ";

		salida << "Numero: " << unNodoInterior->number << "  Nivel: " << unNodoInterior->level << "  Cant.Elem: " << unNodoInterior->keyMount
			   << "  Esp.Libre: " << TreeConstraits::getEfectiveSizeNode() - unNodoInterior->occupiedSpace << endl;

		for (int posicion = 0; posicion <= unNodoInterior->keyMount; ++posicion) {

			if (posicion < unNodoInterior->keyMount) {
				Key unaClave = unNodoInterior->keys[posicion];

				for(int i=0; i<(tabulacion+1) ; i++)
					salida << "  ";
				salida << "(";
				if(keyText)
				{
					salida << unaClave.toString();
				}
				else
				{
					ByteString clave(unaClave.toString());
					salida << Utility::toString(clave.readAsInt(0));
				}
				salida << ")";
				salida << unaClave.getSize();
				salida << "  ";
				salida << unNodoInterior->getSons()[posicion] << "   ";
			}
		}
		for (int posicion = 0; posicion <= unNodoInterior->keyMount; ++posicion) {
			Node *hijo = hidratateNode(unNodoInterior->sons[posicion]);
			exportNode(salida, hijo, tabulacion + 2,keyText,textContent);
			if (hijo)
				freeNodeMemory(hijo);
		}
		for(int i=0; i<tabulacion ; i++)
			salida << "  ";
		salida << endl;
	}
}


void BPlusTree::persistNode(Node* unNodo) {

	if (fileBlockManager->getFileSize() <= unNodo->number * (TreeConstraits::getFullSizeNode())){
		fileBlockManager->insertBlock();
	}
	fileBlockManager->saveBlock(unNodo->number, unNodo->Serialize());
}


Node* BPlusTree::hidratateNode(int numeroDeNodo) {

	ByteString byteStr = this->fileBlockManager->readBlock(numeroDeNodo);
	if (byteStr.isEmpty()) {
		return NULL;
	} else {
		int nivel = byteStr.readAsInt(0);
		if (nivel == 0) {
			LeafNode *nuevoNodoHoja = createLeafNode();
			nuevoNodoHoja->Hidratate(byteStr);
			nuevoNodoHoja->number = numeroDeNodo;
			return nuevoNodoHoja;
		} else {
			InnerNode *nuevoNodoInterior = createInnerNode(nivel);
			nuevoNodoInterior->Hidratate(byteStr);
			nuevoNodoInterior->number = numeroDeNodo;
			return nuevoNodoInterior;
		}
	}
}


int BPlusTree::getNodeNumberToSeize() {
	int numeroDeNodo;
	if (freeNodes.empty()) {
		this->NodesMount++;
		numeroDeNodo = this->NodesMount-1;
	} else {
		numeroDeNodo = freeNodes.back();
		freeNodes.pop_back();
		serializeDataConfig();
	}
	return numeroDeNodo;
}


void BPlusTree::serializeDataConfig() {
	vector<int>::iterator it = freeNodes.begin();
	string nombreArchivo = fileBlockManager->getPath() + ".cnf";
	ofstream ofs(nombreArchivo.c_str());

	if (!ofs.fail()) {

		ofs.write((char*)(&firstLeaf),sizeof(int));

		while (it != freeNodes.end()) {
			int numeroDeNodo = *it;
			ofs.write((char*)(&numeroDeNodo),sizeof(int));
			it++;
		}
		ofs.close();
	}
}


void BPlusTree::hidratateDataConfig() {

	freeNodes.clear();
	string nombreArchivo = fileBlockManager->getPath() + ".cnf";
	ifstream ifs(nombreArchivo.c_str());
	ifs.seekg(0, ios::end);
	int tamanio = ifs.tellg();
	ifs.seekg(0, ios::beg);

	if (!ifs.fail()) {

		char* sPrimeraHoja = new char[sizeof(int)];
		ifs.read(sPrimeraHoja,sizeof(int));
		firstLeaf = *((int*)sPrimeraHoja);
		delete[] sPrimeraHoja;

		while (ifs.tellg() < tamanio) {
			char* s = new char[sizeof(int)];
			ifs.read(s,sizeof(int));
			int numeroDeNodo = *((int*)s);
			freeNodes.push_back(numeroDeNodo);
			delete[] s;
		}
		ifs.close();
	}
}
