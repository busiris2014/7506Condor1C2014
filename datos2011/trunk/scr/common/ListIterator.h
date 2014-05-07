#ifndef LISTITERATOR_H_
#define LISTITERATOR_H_
#include "ListNode.h"
template<class C> class List;

template<class C> class ListIterator{
private:
	List<C>* list;
	ListNode<C>* actualNode;
public:

	//Constructor
	ListIterator(List<C>* list);
	
	//Destructor
	virtual ~ListIterator();

	//Devuelve TRUE si tiene un elemento a continuacion
	bool hasNext();
	
	//Devuelve los data del next nodo
	C next();
};



template<class C> ListIterator<C>::ListIterator(List<C>* list){
	this->list = list;
	actualNode = list->getFirstNode();
}

template<class C> bool ListIterator<C>::hasNext(){
	return (actualNode != NULL);
}

template<class C> C ListIterator<C>::next(){
	C retorno = actualNode->getData();
	actualNode = actualNode->getNext();
	return retorno;
}

template<class C> ListIterator<C>::~ListIterator()
{
}

#endif /*LISTITERATOR_H_*/
