#ifndef MEDIATOR_H_
#define MEDIATOR_H_

#include "../common/Word.h"
#include "../common/BookGlobalWeight.h"
#include "../parser/FileParser.h"
#include "../Business/DataBookManager.h"
#include "../Business/IndexWrapper.h"
//#include "../btree/OcurrenceTree.h"
#include <set>
#include <list>
#include <map>
#include "../common/ListOfPositions.h"
#include "../common/RankingBook.h"

class Mediator {

private:

	FileParser* fileParser;
	DataBookManager* dataBookManager;
	IndexWrapper* indexWrapper;
	BPlusTree* infinityNormIndex;
	BPlusTree* vocabularyIndex;
	ClassifBPlusTree* ocurrenceTree;
	fstream fileAutoIncInteger;
	int autoIncInteger;

public:

	static enum fileItem {NORMA_INFINITO = 0, TERMINOS = 1, OCURRENCIA_POSICIONAL = 2, LISTAS = 3} tFileItem;
	Mediator();
	~Mediator();
	Book* getBook(string pathFile);
	bool addBook(Book* book);
	bool removeBook(int ID);
	string getListOfTakenBooks();
	bool indexingListOnIndex(IndexWrapper::indexItem);
	void initializeIndex(IndexWrapper::indexItem item, string filepath,int maxSize);
	void closeIndex(IndexWrapper::indexItem item);
	ByteString getBookFromFile(int id);
	void exportToPlainText(IndexWrapper::indexItem index, string toFile);
	void addEditorial(string editorial);
	string search(IndexWrapper::indexItem indexId, string toSearch);
	list<int>* findCoincidences(list<Word*> words);
	list<int>* findNotCoincidences(list<Word*> words, list<int>* coincidenceDocs);
	list<RankingBook>* findTermProxIdBooks(list<int>* idBooks,list<Word*> idTerms);
	list<BookGlobalWeight*>* orderBookByGlobalWeight(list<int>* idBooks, list<Word*> words, int totalBooks);
	double calculateBookGlobalWeight(int idBook, int infinityNorm, int totalBooks, int numberDocsWithWord, int repetition);
	int getInfinityNorm(int idBook);
	list<int>* getListOfPositions(int idBook, int idTerm);
	void exportToTXT(fileItem item,string filepath);
	void exportInvertedList(string pathname);
};

#endif /* MEDIATOR_H_ */
