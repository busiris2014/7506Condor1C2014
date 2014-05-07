#ifndef LISTNODE_H_
#define LISTNODE_H_
#include <iostream>

template<class C> class ListNode
{
private:
	ListNode<C>* next;
	ListNode<C>* previous;
	C data;
	
public:

	//Constructor
	ListNode(){
		this->data = NULL;
		this->previous = NULL;
		this->next = NULL;	
	}

	ListNode(C _data){
		this->data = _data;
		this->previous = NULL;
		this->next = NULL;	
	}

	ListNode(C _data, ListNode* _previous, ListNode* _next){
		this->data = _data;
		this->previous = _previous;
		this->next = _next;
	}

	ListNode(const ListNode& node){
		this->data = node.getData;
		this->previous = node->getAnterior();
		this->next = node->getSiguiente();
	}
	
	//Destructor
	virtual ~ListNode(){};

	//Devuelve true si tiene un elemento siguiente
	bool hasNext();
	
	//Devuelve true si tiene un elemento anterior
	bool hasPrevious();
	

	// Get and Set del nodo siguiente
	void setNext(ListNode* node){
		this->next = node;	
	}
	
	ListNode* getNext(){
		return this->next;
	}
	
	// Get and Set del nodo anterior
	void setPrevious(ListNode* node){
		this->previous = node;	
	}
	
	ListNode* getPrevious(){
		return this->previous;
	}
	
	// Get and Set del contenido del nodo
	void setData(C _data){
		this->data = _data;
	}

	C getData();
};

template<class C> bool ListNode<C>::hasNext(){
	if (next != NULL) return true;
	else return false;	
}
	
template<class C> bool ListNode<C>::hasPrevious(){
	if (previous != NULL) return true;
	else return false;	
}
template<class C> C ListNode<C>::getData(){
		return this->data;
}
#endif /*NODO_H_*/
