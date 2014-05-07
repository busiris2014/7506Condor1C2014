#ifndef COMPARER_H_
#define COMPARER_H_

template<class C> class Comparer
{
public:
	Comparer(){};
	//devuelve 0 si A=B, 1 si A>B, -1 A<B
	virtual int compare( C A, C B) = 0;
	virtual ~Comparer(){};
};

#endif /*COMPARER_H_*/
