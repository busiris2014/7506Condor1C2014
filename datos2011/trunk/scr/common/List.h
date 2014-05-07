#ifndef LIST_H__
#define LIST_H__
#include "Comparer.h"
#include "ComparerNull.h"
#include "ListIterator.h"

template<class C> class List
{
private:
	ListNode<C>* firstNode;
	ListNode<C>* lastNode;
	Comparer<C>* comparer;
	int listSize;
	
	//Insertan nodos en la list
	void insertNodeAtBegining(ListNode<C>* node);
	void insertNodeAtEnd(ListNode<C>* node);
	void insertNode(ListNode<C>* newNode, ListNode<C>* nodeReference);
	
	//Busca un nodo en la list
	ListNode<C>* findNode(C data);
	
public:

	//Contructor
	List();
	List(Comparer<C> *comparador);

	//Destructor
	virtual ~List();

	//Devuelve true si la list no tiene ningun nodo
	bool isEmpty();
	
	//Devuelve la cantidad de elementos de la list
	int getSize();
	
	//Devuelve el primer data de la list
	C getFirst();

	//Devuelve el ultimo data de la list
	C getLast();

	//Inserta un data a la list
	void insertData(C data);
	
	//Elimina un data a la list
	void removeData(C dato);
	
	//Elimina un nodo de la list
	void removeNode(ListNode<C>* node);

	//Elimina todos los elementos de la list
	void clean();
	
	//Devuelve un getIterator a la list
	ListIterator<C> getIterator();

	//Devuelve el primer nodo de la list
	ListNode<C>* getFirstNode();

	//Devuelve el ultimo nodo de la list
	ListNode<C>* getLastNode();

	friend class ListIterator<C>;
};

template<class C> List<C>::List(){
	firstNode = NULL;
	lastNode = NULL;
	comparer = new ComparerNull<C>();
	listSize = 0;
}

template<class C> List<C>::List(Comparer<C> *comparador){
	firstNode = NULL;
	lastNode = NULL;
	this->comparer = comparador;
	listSize = 0;
}

template<class C> bool List<C>::isEmpty(){
	return listSize == 0;	
}

template<class C> int List<C>::getSize(){
	return listSize;
}

template<class C> C List<C>::getFirst(){
	return firstNode->getData();
}

template<class C> C List<C>::getLast(){
	return lastNode->getData();
}

template<class C> void List<C>::insertNodeAtBegining(ListNode<C>* node){
	if(firstNode){
		firstNode->setPrevious(node);
	}else{
		lastNode = node;	
	}
	node->setNext(firstNode);
	node->setPrevious(NULL);
	firstNode = node;
}

template<class C> void List<C>::insertNodeAtEnd(ListNode<C>* node){
	if(lastNode) lastNode->setNext(node);
		else firstNode = node;
	node->setPrevious(lastNode);
	node->setNext(NULL);
	lastNode = node;	
}

template<class C> void List<C>::insertNode(ListNode<C>* newNode, ListNode<C>* nodeReference){
	if(nodeReference->hasPrevious()) nodeReference->getPrevious()->setNext(newNode);
		else firstNode = newNode;
	newNode->setPrevious(nodeReference->getPrevious());
	nodeReference->setPrevious(newNode);
	newNode->setNext(nodeReference);
}

template<class C> ListNode<C>* List<C>::findNode(C data){
	ListNode<C>* node = firstNode;
	bool found = false;
	if(node){
		do{
			if(node->getData() == data) found = true;
				else node = node->getNext();
		}while(node && !found);	
	}
	if(found) return node;
		else return NULL;
}

template<class C> void List<C>::removeNode(ListNode<C>* node){
	if(node){
	if(node->hasPrevious()) node->getPrevious()->setNext(node->getNext());
		else firstNode = node->getNext();
	if(node->hasNext()) node->getNext()->setPrevious(node->getPrevious());
		else lastNode = node->getPrevious();
	delete node;	
	listSize--;
	}
}

template<class C> ListNode<C>* List<C>::getFirstNode(){
		return firstNode;	
}

template<class C> ListNode<C>* List<C>::getLastNode(){
		return lastNode;
}

template<class C> void List<C>::insertData(C data){
	ListNode<C>* newNode = new ListNode<C>(data,NULL,NULL);
	if (this->isEmpty()){
		insertNodeAtBegining(newNode);	
	}else{
		ListNode<C>* actualNode = firstNode;
		bool foundPosition = false;
		//si encuentra posicion lo inserto ahi, sino lo inserto al final
		while(actualNode && !foundPosition){
			if (comparer->compare(newNode->getData(),actualNode->getData()) == 1) foundPosition = true;
			else actualNode = actualNode->getNext();
		}
		if(foundPosition){
			insertNode(newNode,actualNode);
		}else{
			insertNodeAtEnd(newNode);
		}
	}
	listSize++;	
}

template<class C> ListIterator<C> List<C>::getIterator(){
	ListIterator<C> iterador(this);
	return iterador;	
}

template<class C> void List<C>::removeData(C dato){
	ListNode<C>* nodo = findNode(dato);
	removeNode(nodo);
}

template<class C> void List<C>::clean(){
	while(firstNode){
		removeNode(firstNode);
	}
}

template<class C> List<C>:: ~List(){
	this->clean();
	delete this->comparer;
}

#endif /*LIST_H__*/
