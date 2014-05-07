
#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_

#include "../common/Key.h"
#include "../common/Record.h"
#include "../common/ByteString.h"
#include "InnerNode.h"
#include "LeafNode.h"
#include "BPlusTreeIterator.h"
#include "Result.h"
#include "../common/FileBlockManager.h"
#include "../common/Constantes.h"
#include "KeyComparator.h"
#include "TreeConstraits.h"

#include <fstream>
#include <vector>

using namespace std;


class BPlusTree {

	friend class Node;
	friend class InnerNode;
	friend class LeafNode;

protected:
	Node* root;
	int firstLeaf;
	int NodesMount;
	int keyTopSize;
	vector<int> freeNodes;
	FileBlockManager* fileBlockManager;

public:
	//Este constructor no se debe utilizar, lo llama el constructor de la Clase ClassifBPlusTree
	BPlusTree();
	/*
	 * Constructor
	 * Recibe por parametro la ruta del archivo donde se va a guardar la estructura, y el tamanio
	 * maximo de clave que puede almacenar.
	 * Pre:
	 * Pos: Queda creada una instancia de ArbolBMas.
	 * 		Los archivos correspondientes al arbol y a datos de control
	 * 		quedan abiertos y creados si no existieran.
	 */
	BPlusTree(string filePath, int maxSizeKey);

	/*
	 * Destructor
	 */
	virtual ~BPlusTree();

	/*
	 * Primitiva que permite ingresar un dato (registro) a la estructura
	 * Pre: El registro debe existir y ser de tipo Registro. Recibo la clave y el Registro.
	 * Pos: Devuelve TRUE si se inserto correctamente y FALSE en caso contrario.
	 */
	virtual bool add(Record* r);

	/*
	 * Primitiva que devuelve un IteradorArbolBMas al primer nodo hoja del arbol.
	 * Pre: el arbol existe
	 * Pos: Devuelve el iterador en caso de que exista algun registro en el arbol. En caso
	 * 		contrario devuelve NULL.
	 */
	virtual BPlusTreeIterator* begin();

	/*
	 * Primitiva que va a buscar una clave o cadena de caracteres en la estructura o archivo
	 * Pre: Recibo la clave del registro a buscar.
	 * Pos: Si lo encontro, devuelve el registro, sino, devuelve NULL.
	 */
	virtual pair<Record*, BPlusTreeIterator*> search(Key k);

	/*
	 * Primitiva que elimina un registro de la estructura.
	 * Pre: Paso la clave del registro a eliminar.
	 * Pos: Devuelve TRUE si se pudo borrar correctamente y FALSE en caso contrario.
	 */
	virtual bool removeKey(Key k);

	/*
	 * Primitiva que pasa todos los datos de la estructura en un archivo de texto plano
	 * para que pueda ser analizado en una observacion inmediata.
	 * Pre: ruta_archivo debe contener caracteres validos para un nombre de archivo.
	 * Pos: Crea un archivo de texto con los todos los datos de la estructura.
	 */
	virtual void exportToPlainText(string filePath,bool keytext, bool textContent);
	virtual void exportToPlainTextNoOrder(string filePath);

	/*
	 * Primitiva que devuelve un Nodo a partir del numero de nodo pasado por parametro.
	 * Pre: numeroDeNodo debe ser un numero de nodo valido.
	 * Pos: Devuelve un puntero al Nodo buscado. Si el numero de nodo es invalido
	 * 		devuelve NULL.
	 */
	virtual Node* readNode(int nodeNumber);

	virtual int blocksCount();

protected:

	/*
	 * Indica si key es menor que otherKey.
	 */
	virtual bool minorKey(Key key, Key otherKey);

	/*
	 * Indica si key es menor o igual que otherKey.
	 */
	virtual bool minorOrEqualKey(Key key, Key otherKey);

	/*
	 * Indica si key es igual a otherKey.
	 */
	virtual bool equalKey(Key key, Key otherKey);

	/*
	 * Crea una instancia de un NodoHoja.
	 */
	virtual LeafNode* createLeafNode();

	/*
	 * Crea una instancia de un NodoInterior seteando un nivel recibido por parametro
	 */
	virtual InnerNode* createInnerNode(int level);

	/*
	 * libera la memoria ocupada por aNode.
	 */
	virtual void freeNodeMemory(Node *aNode);

	/*
	 * Devuelve la posicion en el nodo 'aNode' de la clave buscada. La primera posicion en el nodo es la cero.
	 * Se usa tanto para busquedas, modificaciones y bajas, como para altas, donde la posicion devuelta es donde
	 * deberia insertarse el nuevo elemento.
	 */
	virtual int getPosition(Node *aNode, Key key);

	/*
	 * Metodo recursivo que se encarga de insertar un elemento en el arbol, y realizar los splits necesarios.
	 * nodoCorriente es el nodo que se esta procesando.
	 * clave y dato corresponden al elemento a insertar.
	 * En clavePromocion se devuelve la clave que se actualiza en el nodo padre.
	 * En nuevoNodo se devuelve el nodo que se genera en el caso de un split.
	 * El metodo devuelve un boolean que indica si se pudo realizar o no la insercion.
	 */
	virtual bool recursiveInsert(Node* currentNode, Key k, ByteString byteData, Key* toPromoteKey, Node** newNode);

	/*
	 * Se encarga de hacer un split de unNodoHoja, generando un nuevoNodoHoja y la clavePromocion que es la que se actualiza en el
	 * nodo padre.
	 */
	virtual void splitLeafNode(LeafNode* leafNode, Key* toPromoteKey, Node** newLeafNode);

	/*
	 * Se encarga de hacer un split de unNodoInterior, generando un nuevoNodoInterior y la clavePromocion que es la que se actualiza en el
	 * nodo padre.
	 */
	virtual void splitInnerNode(InnerNode* innerNode, Key* toPromoteKey, Node** newInnerNode, int newPosition);

	/*
	 * Metodo recursivo que se encarga de borrar un elemento del arbol, y realizar los balanceos o fusiones correspondientes.
	 * clave es la clave del elemento a borrar.
	 * nodoCorriente es el nodo que se esta procesado.
	 * nodoIzquierda y nodoDerecha son los nodos que se encuentran al los lados del corriente, NULL en caso de no existir ninguno.
	 * nodoPadreIzquierda y nodoPadreDerecha representan al nodoPadre en la mayoria de los casos, y en los casos en que el nodo en cuestion
	 * se encuentra en alguno de los extremos, representan al padre del padre, segun se encuentre en el extremo izquierdo o derecho.
	 * nodoPadre representa al padre del corriente y posicionPadre la ubicacion del nodoCorriente en el padre.
	 * Puede devolver OK, NO_ENCONTRADO, ACTUALIZAR_ULTIMA_CLAVE o FUSION_NODOS segun sea el caso.
	 */
	virtual Result recursiveRemove(Key k, Node* currentNode, Node* leftNode, Node* rightNode,
			InnerNode* leftFatherNode, InnerNode* rightFatherNode, InnerNode* fatherNode, int fatherPosition);

	/*
	 * Se encarga de fusionar dos nodos hoja.
	 * hojaIzquierda y hojaDerecha son los nodos en cuestion. En hoja izquierda se pasan todos los elementos de hojaDerecha.
	 * Devuelve Resultado::FUSION_NODOS.
	 */
	virtual Result leafNodeFusion(LeafNode* leftLeaf, LeafNode* rightLeaf);

	/*
	 * Se encarga de fusionar dos nodos interiores.
	 * nodoIzquierda y nodoDerecha son los nodos en cuestion. En nodoIzquierda se pasan todos los elementos de nodoDerecha.
	 * Si se produce la fusion devuelve Resultado::FUSION_NODOS, sino Resultado::OK
	 */
	virtual Result innerNodeFusion(InnerNode* leftInner, InnerNode* rightInner, InnerNode* fatherNode, int fatherPosition);

	/*
	 * Se encarga de balancear un nodo hoja derecho, con su izquierdo, pasando elementos a este ultimo.
	 * hojaIzquierda y hojaDerecha son los nodos en cuestion, nodoPadre es el padre de ambos y posicionPadre es la ubicacion en el
	 * padre, del nodo que contiene al elemento a ser borrado.
	 * Si cambia la ultima clave del padre (lo que implicaria cambiar la clave tambien en el padre del padre) se devuelve
	 * Resultado::ACTUALIZAR_ULTIMA_CLAVE, caso contrario se devuelve Resultado::OK
	 */
	virtual Result redistributeLeftLeaf(LeafNode* leftLeaf, LeafNode* rightLeaf, InnerNode* fatherNode, int fatherPosition);

	/*
	 * Se encarga de balancear un nodo interior derecho, con su izquierdo, pasando elementos a este ultimo.
	 * nodoIzquierda y nodoDerecha son los nodos en cuestion, nodoPadre es el padre de ambos y posicionPadre es la ubicacion en el
	 * padre, del nodo actual.
	 */
	virtual void redistributeLeftInner(InnerNode* leftInner, InnerNode* rightInner, InnerNode *fatherNode, int fatherPosition);

	/*
	 * Se encarga de balancear un nodo hoja izquierdo, con su derecho, pasando elementos a este ultimo.
	 * hojaIzquierda y hojaDerecha son los nodos en cuestion, nodoPadre es el padre de ambos y posicionPadre es la ubicacion en el
	 * padre, del nodo que contiene al elemento a ser borrado.
	 */
	virtual void redistributeRightLeaf(LeafNode* leftLeaf, LeafNode* rightLeaf, InnerNode* fatherNode, int fatherPosition);

	/*
	 * Se encarga de balancear un nodo interior izquierdo, con su derecho, pasando elementos a este ultimo.
	 * nodoIzquierda y nodoDerecha son los nodos en cuestion, nodoPadre es el padre de ambos y posicionPadre es la ubicacion en el
	 * padre, del nodo actual.
	 */
	virtual void redistributeRightInner(InnerNode* leftInner, InnerNode* rightInner, InnerNode* fatherNode, int fatherPosition);

	/*
	 * Persiste un nodo en el archivo del arbol b+.
	 */
	virtual void persistNode(Node* n);

	/*
	 * Hidrata un nodo del archivo del arbol b+, numeroDeNodo multiplicado al tamanio del nodo es el offset en el archivo.
	 */
	virtual Node* hidratateNode(int nodeNumber);

	/*
	 * Se encarga de mostrar la estructura del arbol en el stream de salida pasado por parametro. Es un metodo recursivo, unNodo es
	 * el nodo corriente, la tabulacion va cambiando segun el nivel.
	 */
	virtual void exportNode(ofstream& outStream, Node* n, int tab,bool keyText,bool textContent);

	/*
	 * Devuelve el numero correspondiente a un nuevo nodo. En el caso de que hayan quedado nodos libres se devuelve
	 * un numero de nodo del vector de nodosLibres, en caso contrario se genera un nuevo numero de nodo.
	 */
	virtual int getNodeNumberToSeize();

	/*
	 * Persiste el numero de nodo de la primera hoja y el vector de nodos libres en el archivo de configuracion.
	 */
	virtual void serializeDataConfig();

	/*
	 * Levanta el archivo de configuracion y a partir de el setea la primera hoja y genera el vector de nodosLibres.
	 */
	virtual void hidratateDataConfig();

};

#endif // _BPLUSTREE_H_
