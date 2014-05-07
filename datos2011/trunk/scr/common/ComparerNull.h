#ifndef COMPARERNULL_H_
#define COMPARERNULL_H_
#include "Comparer.h"

/* Plantilla que representa un comparer que para cualquier
 * elemento, el resultado de la comparacion sera la igualdad */
template<class C> class ComparerNull: public Comparer<C>
{
public:
	virtual int compare(C A,C B);
};

template<class C> int ComparerNull<C>::compare(C A,C B){
		return 0;
}

#endif /*COMPARERNULL_H_*/
