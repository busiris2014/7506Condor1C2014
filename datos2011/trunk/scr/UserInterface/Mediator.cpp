#include "Mediator.h"
#include "../common/List.h"
#include "../common/ListIterator.h"
#include "unistd.h"
#include <math.h>
#include <list>
using namespace std;

Mediator::Mediator()
{
	//FIXME
	this->dataBookManager = new DataBookManager("ArchivoDeLibros");
	this->fileParser = new FileParser();
	this->indexWrapper = IndexWrapper::getInstanceOfIndexWrapper();

	// ---------------------------------------------------------------------------------------------------------------------------- //
	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.index");

	this->vocabularyIndex = new BPlusTree(indexPath + "indexVocabulario", 50);
	this->infinityNormIndex = new BPlusTree(indexPath + "indexNormaInfinito", 50);
	// ---------------------------------------------------------------------------------------------------------------------------- //

	// ---------------------------------------------------------------------------------------------------------------------------- //
	//string ocurrencePath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.control") + "ocurrencias";
	//this->ocurrenceTree = new OcurrenceTree(ocurrencePath, 50);
	string ocurrencePath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.index");
	this->ocurrenceTree = new ClassifBPlusTree(ocurrencePath  + "indexOcurrencias", 50);
	// ---------------------------------------------------------------------------------------------------------------------------- //

	// ---------------------------------------------------------------------------------------------------------------------------- //
	string incrementPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.control") + "IntAutoIncVocabulario.inc";
	this->fileAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
	bool open = this->fileAutoIncInteger.is_open();
	if(!open)
	{
		fopen(incrementPath.c_str(),"w+");
		this->fileAutoIncInteger.open(incrementPath.c_str(), ios::in | ios::out | ios::binary);
		this->fileAutoIncInteger.seekp(0);
		int cero = 0;
		this->fileAutoIncInteger.write((char*)&cero,sizeof(int));
	}

	this->fileAutoIncInteger.seekg(0);
	int incInteger;
	this->fileAutoIncInteger.read((char*)&incInteger,sizeof(int));
	this->autoIncInteger = incInteger;
	// ---------------------------------------------------------------------------------------------------------------------------- //
}

Mediator::~Mediator()
{
	delete this->dataBookManager;
	delete this->fileParser;
	// --------------------------------------------------------- //
	delete this->vocabularyIndex;
	delete this->infinityNormIndex;
	// --------------------------------------------------------- //
	delete this->ocurrenceTree;
	// --------------------------------------------------------- //
	this->fileAutoIncInteger.seekp(0);
	int lastId = this->autoIncInteger;
	this->fileAutoIncInteger.write((char*)&lastId,sizeof(int));
	this->fileAutoIncInteger.close();
	// --------------------------------------------------------- //
	//delete this->indexWrapper;
}

Book* Mediator::getBook(string pathFile)
{
	bool done = this->fileParser->parse(pathFile);
	Book* book = NULL;
	if(done)
	{
		book = this->fileParser->getBook();
	}
	return book;
}


bool Mediator::addBook(Book *book)
{
	bool done = false;
	if(book != NULL)
	{
		int ID = this->dataBookManager->addBook(book->Serialize());
		if(ID>0)
			done = true;
	}
	return done;
}

bool Mediator::removeBook(int ID)
{
	bool done = false;
	if(ID>0)
	{
		ByteString byteString = this->dataBookManager->getBook(ID);
		if(!byteString.isEmpty())
		{
			bool isRemoved = this->dataBookManager->removeRecord(ID);
			if(isRemoved)
			{
				Book book;
				book.Hidratate(byteString);
				this->indexWrapper->removeFromAll(book);
				done = true;
			}
		}
	}
	return done;
}

string Mediator::getListOfTakenBooks()
{
	string listOfTakenBooks = this->dataBookManager->getlistOfTakenBooks();
	return listOfTakenBooks;
}

void Mediator::initializeIndex(IndexWrapper::indexItem item, string filepath,int maxSize)
{
	//cout << "Start Index Initilizer: " << Utility::getDate();
	this->indexWrapper->createIndex(filepath,item,maxSize);
	//cout << "End Index Initilizer: " << Utility::getDate() << endl;
}

void Mediator::closeIndex(IndexWrapper::indexItem item)
{
	//cout << "Start Index Closing: " << Utility::getDate();
	this->indexWrapper->closeIndex(item);
	//cout << "End Index Closing: " << Utility::getDate() << endl;
}

bool Mediator::indexingListOnIndex(IndexWrapper::indexItem item)
{
	bool done = false;
	List<int>* flagsLs = this->dataBookManager->getListToFileFlag(item);
	if(!flagsLs->isEmpty())
	{
		cout << "Process Started: " << Utility::getDate() << endl;
		ListIterator<int> it = flagsLs->getIterator();
		int ID;
		while(it.hasNext())
		{
			ID = it.next();
			ByteString bs = this->dataBookManager->getBook(ID);
			Book book;
			book.Hidratate(bs);

			string clave;
			Record* record;
			ByteString bsIdBook;
			ByteString* bsId;
			bsIdBook.insertLast(&ID,sizeof(int));
			Key* key;
			switch(item){
			case IndexWrapper::AUTOR :
										clave = book.getAuthor();
										cout << "Book begin process: " << book.getTitle() << " - Author:" << clave << " - Start:" << Utility::getDate();

										key = new Key(clave);
										bsId = new ByteString(bsIdBook);
										record = new Record(key,bsId);
										done = this->indexWrapper->add(record,item);
										break;
			case IndexWrapper::EDITORIAL :
										clave = book.getEditorial();
										cout << "Book begin process: " << book.getTitle() << " - Editorial:" << clave << " - Start:" << Utility::getDate();

										key = new Key(clave);
										bsId = new ByteString(bsIdBook);
										record = new Record(key,bsId);
										done = this->indexWrapper->add(record,item);
										break;
			case IndexWrapper::TITULO :
										clave = book.getTitle();
										cout << "Book begin process: " << book.getTitle() << " - Start:" << Utility::getDate();

										key = new Key(clave);
										bsId = new ByteString(bsIdBook);
										record = new Record(key,bsId);
										done = this->indexWrapper->add(record,item);
										break;
			case IndexWrapper::PALABRAS :
										this->fileParser->setWords(book.getText());

										// ----------------------------------------------------------------------------------------------- //
										// Se incorpora la norma infinito del documento
										// ----------------------------------------------------------------------------------------------- //
										Key* keyIdBook = new Key(bsIdBook.toString());

										ByteString* bsInfinityNorm = new ByteString();
										unsigned int infinityNorm = this->fileParser->getInfinityNorm();
										bsInfinityNorm->insertLast(&infinityNorm, sizeof(unsigned int));

										Record* recInfinityNorm = new Record(keyIdBook, bsInfinityNorm);
										this->infinityNormIndex->add(recInfinityNorm);
										delete recInfinityNorm;
										// ----------------------------------------------------------------------------------------------- //

										map<string,Term*> terms = this->fileParser->getTerms();
										cout << "Book begin process: " << book.getTitle() << " - Words Count:" << Utility::intToString(terms.size()) << " - Start:" << Utility::getDate();
										int i = 0;
										for(map<string,Term*>::iterator it = terms.begin(); it != terms.end(); ++it)
										{
											i++;
											// ------------------------ //
											key = new Key(it->first);
											string word = it->first;
											list<int>* listDocuments = this->indexWrapper->searchAllIds(key, IndexWrapper::PALABRAS);
											int idTerm;

											// Si no tiene datos --> la palabra no existe en el hash
											if (listDocuments->size() == 0)
											{
												//delete poddo
												delete listDocuments;

												// Se agrega la palabra al vocabulario y se obtiene su id
												int possibleID = this->autoIncInteger + 1;

												// La clave es el id de termino y el dato la palabra
												ByteString bsIdTerm;
												bsIdTerm.insertLast(&possibleID, sizeof(int));
												Key* keyIdTerm = new Key(bsIdTerm.toString());

												ByteString* bsTerm = new ByteString();
												bsTerm->insertLast(word);

												Record* recVocabulary = new Record(keyIdTerm, bsTerm);
												this->vocabularyIndex->add(recVocabulary);
												delete recVocabulary;

												idTerm = possibleID;
												this->autoIncInteger++;
											}
											else
											{
												// Devuelve en la ultima posicion el id del termino y en las restantes los id de documentos
												Word* newWord = new Word(word, listDocuments);
												idTerm = newWord->getIdWord();
												delete newWord; // se elimina la lisdDocuments tambien
											}

											//Aca se agrega las ocurrencias del termino en el libro
											list<unsigned int> listOfPositions = it->second->getPositions();

											ByteString bs;
											bs.insertLast(&ID,sizeof(int));
											bs.insertLast(&idTerm,sizeof(int));
											Record* r = new Record();
											r->setKey(new Key(bs.toString()));

											this->ocurrenceTree->addList(r,listOfPositions);

											listOfPositions.clear();
											delete r;

											// ------------------------ //
											bsId = new ByteString(bsIdBook);
											bsId->insertLast(&idTerm, sizeof(int));
											record = new Record(key,bsId);

											done = this->indexWrapper->add(record,item);
											delete record;
										}

										break;
			}
			cout << "Book processed: " << book.getTitle() << " - Finish:" << Utility::getDate() << endl;
		}
		cout << "Process Ended: " << Utility::getDate();
	}

	//delete poddo
	if (flagsLs != NULL)
		delete flagsLs;

	return done;
}

ByteString Mediator::getBookFromFile(int id)
{
	ByteString bs;
	bs = this->dataBookManager->getBook(id);
	return bs;
}

void Mediator::exportToPlainText(IndexWrapper::indexItem index, string toFile){
	IndexWrapper::getInstanceOfIndexWrapper()->exportIndex(toFile, index);
}

void Mediator::addEditorial(string editorial)
{
	this->fileParser->addEditorial(editorial);
}


/**************************QUERYS*****************************/

string Mediator::search(IndexWrapper::indexItem index, string toSearch)
{
	Key* k = NULL;
	string toReturn;
	list<int>* bookIds;
	//string separator;
	//list<string> wordsToFind;
	FileParser* fp;
	map<string, Term*> wordsToFind;
	list<Word*> words;
	list<int>* coincidenceDocs = NULL;
	list<int>* notCoincidenceDocs = NULL;
	list<BookGlobalWeight*>* bookByGlobalWeight = NULL;
	bool doSearch = false;
	int totalBooks;

	switch(index){
		case IndexWrapper::EDITORIAL:
			k = new Key(toSearch);
			bookIds = this->indexWrapper->searchAllIds(k, index);
			break;
		case IndexWrapper::AUTOR:
			k = new Key(toSearch);
			bookIds = this->indexWrapper->searchAllIds(k, index);
			break;
		case IndexWrapper::TITULO:
			k = new Key(toSearch);
			bookIds = this->indexWrapper->searchAllIds(k, index);
			break;
		case IndexWrapper::PALABRAS:
		{
			bookIds = new list<int>();

			//separator = " ";
			//wordsToFind = Utility::split(toSearch, separator);

			fp = new FileParser();
			fp->setWords(toSearch);
			wordsToFind = fp->getTerms();
			delete fp;

			// Se recorre todas las palabras de las busqueda, y se buscan en el hash
			for(map<string, Term*>::iterator it = wordsToFind.begin(); it != wordsToFind.end(); ++it)
			{
				Key* k = new Key((*it).first);
				list<int>* booksWords = this->indexWrapper->searchAllIds(k, index);
				Word* newWord = new Word((*it).first, booksWords);
				words.push_back(newWord);
				if (newWord->getBooks()->size() > 0)
					doSearch = true;
				delete k;
			}

			if (wordsToFind.size()>0 && doSearch)
			{
				//Obtiene los libros en que se encuentran en todas las palabras
				coincidenceDocs = this->findCoincidences(words);
				if (coincidenceDocs->size() > 0)
				{
					list<RankingBook>* listRanking;
					listRanking = findTermProxIdBooks(coincidenceDocs,words);
					list<RankingBook>::iterator itRanking;

					cout << "Ids book Encontrados por Terminos Proximos: ";
					for(itRanking=listRanking->begin();itRanking!=listRanking->end();++itRanking)
					{
						int idBook = (*itRanking).getIdBook();
						cout << idBook << " ";
						bookIds->push_back(idBook);
					}
					cout << endl;
				}

				//Obtiene los libros que se encuentran solo algunas de las palabras
				notCoincidenceDocs = this->findNotCoincidences(words, coincidenceDocs);
				if (notCoincidenceDocs->size() > 0)
				{
					//Ordena los documentos que no tienen coincidencia total en los terminos segun el peso global
					totalBooks = coincidenceDocs->size() + notCoincidenceDocs->size();
					bookByGlobalWeight = this->orderBookByGlobalWeight(notCoincidenceDocs, words, totalBooks);

					cout << "Ids book Encontrados por Peso Global: ";
					for(list<BookGlobalWeight*>::iterator itGW = bookByGlobalWeight->begin(); itGW!=bookByGlobalWeight->end(); ++itGW)
					{
						int idBook = (*itGW)->getIdBook();
						cout << idBook << " ";
						bookIds->push_back(idBook);
					}
					cout << endl;
				}
			}

			break;
		}
		default:
			break;
	}

	if(bookIds->size() > 0){
		for (list<int>::iterator it = bookIds->begin(); it != bookIds->end(); ++it)
		{
			ByteString bs = this->dataBookManager->getBook(*it);
			Book b;
			b.Hidratate(bs);
			toReturn.append(b.getBookFormatedText());
		}
	}else
		toReturn = "La busqueda no arrojo resultados";


	delete k;
	return toReturn;
}

list<int>* Mediator::findCoincidences(list<Word*> words)
{
	bool foundIt;
	bool foundItInAll;
	list<int>* coincidenceDocs = new list<int>();

	//si hay mas de una palabra para buscar, si hay solo una se compara por peso global
	if (words.size() > 1)
	{
		//Comparamos la lista de libros de cada palabra
		Word* firstWord = words.front();
		for (list<int>::iterator it = firstWord->getBooks()->begin(); it != firstWord->getBooks()->end(); ++it)
		{
			int idBook = *it;
			foundItInAll = true;

			//Compara contra el resto de palabras
			for (list<Word*>::iterator it2 = words.begin(); it2 != words.end(); ++it2)
			{
				foundIt = false;
				for (list<int>::iterator it3 = (*it2)->getBooks()->begin(); it3 != (*it2)->getBooks()->end(); ++it3)
				{
					int idBook2 = *it3;

					if (idBook == idBook2)
						foundIt = true;
				}

				if (!foundIt)
					foundItInAll = false;

			}

			if (foundItInAll)
				coincidenceDocs->push_back(idBook);

		}
	}

	return coincidenceDocs;
}

list<int>* Mediator::findNotCoincidences(list<Word*> words, list<int>* coincidenceDocs)
{
	bool foundIt;
	bool exists;
	list<int>* notCoincidenceDocs = new list<int>();

	//Compara contra cada una de las palabras
	for (list<Word*>::iterator it = words.begin(); it != words.end(); ++it)
	{
		//compara con la lista de libros de cada una de la palabras
		for (list<int>::iterator it1 = (*it)->getBooks()->begin(); it1 != (*it)->getBooks()->end(); ++it1)
		{
			int idBook = *it1;

			//verifica que no se encuentre enla lista de total coincidencia
			foundIt = false;
			for (list<int>::iterator it2 = coincidenceDocs->begin(); it2 != coincidenceDocs->end(); ++it2)
			{
				int idBook2 = *it2;
				if (idBook == idBook2)
					foundIt = true;
			}

			if (!foundIt)
			{
				//verifica que no se encuentre en la lista de no coincidencias
				exists = false;
				for (list<int>::iterator it3 = notCoincidenceDocs->begin(); it3 != notCoincidenceDocs->end(); ++it3)
				{
					int idBook3 = *it3;
					if (idBook == idBook3)
						exists = true;
				}

				if (!exists)
					notCoincidenceDocs->push_back(idBook);
			}
		}

	}

	return notCoincidenceDocs;
}

list<RankingBook>* Mediator::findTermProxIdBooks(list<int>* idBooks,list<Word*> idTerms)
{
	// si llega aca, estamos seguros que hay al menos 2 terminos a buscar
	list<int>::iterator itIdBooks;
	list<Word*>::iterator itIdTerms;
	list<ListOfPositions> listOfListOfPositions;
	list<RankingBook>* listRankingBook = new list<RankingBook>;
	for(itIdBooks=idBooks->begin();itIdBooks!=idBooks->end();++itIdBooks)
	{
		int idB = *itIdBooks;
		for(itIdTerms=idTerms.begin();itIdTerms!=idTerms.end();++itIdTerms)
		{
			int idBook = *itIdBooks;
			int idTerm = (*itIdTerms)->getIdWord();

			list<int>* listOfPos = getListOfPositions(idBook,idTerm);
			ListOfPositions listOfPositions(listOfPos,idTerm);
			listOfListOfPositions.push_back(listOfPositions);
		}
		list<ListOfPositions>::iterator itLOfPos;
		int minimTotal = 0;

		itLOfPos = listOfListOfPositions.begin();
		ListOfPositions previous = (*itLOfPos);
		itLOfPos++;
		ListOfPositions current = (*itLOfPos);

		for( ; itLOfPos != listOfListOfPositions.end(); ++itLOfPos)
		{
			current = *itLOfPos;
			minimTotal += current.minDistance(previous.getListOfPositions());
			previous = current;
		}
		RankingBook ranking;
		ranking.setIdBook(idB);
		ranking.setDistanceTerms(minimTotal);
		bool insertado = false;
		if (listRankingBook->size() > 0)
		{
			for (list<RankingBook>::iterator it = listRankingBook->begin(); it != listRankingBook->end(); ++it)
			{
				if(!insertado)
				{
					if ((*it).getDistanceTerms() <= ranking.getDistanceTerms())
					{
						--it;
						listRankingBook->insert(it, ranking);
						insertado = true;
					}
				}
			}
			if(!insertado)
				listRankingBook->push_back(ranking);
		}
		else
			listRankingBook->push_back(ranking);

		listOfListOfPositions.clear();
	}
	return listRankingBook;
}

list<BookGlobalWeight*>* Mediator::orderBookByGlobalWeight(list<int>* idBooks, list<Word*> words, int totalBooks)
{
	list<BookGlobalWeight*>* orderDocs = new list<BookGlobalWeight*>();
	list<int>* listOfPos;
	bool insertado =  false;
	int actualIdBook;
	int actualIdTerm;
	int infinityNorm;
	int numberDocsWithWord;
	int repetition;
	Word* word;
	double actualBookGlobalWeight;
	BookGlobalWeight* bookGlobalWeight;

	//recorre cada uno de los libros para calcular su peso global
	for (list<int>::iterator it = idBooks->begin(); it != idBooks->end(); ++it)
	{
		actualIdBook = *it;
		actualBookGlobalWeight = 0;

		//obtiene la norma infinito para el libro actual
		infinityNorm = this->getInfinityNorm(actualIdBook);

		//recorre todas las palabras para ver si estan en el libro actual
		for (list<Word*>::iterator it1 = words.begin(); it1 != words.end(); ++it1)
		{
			word = *it1;
			actualIdTerm = word->getIdWord();

			//obtiene la cantidad de documentos donde aparece la palabra
			numberDocsWithWord = word->getBooks()->size();

			//recorre cada uno de los libros de la palabra para ver si existe el libro actual
			for (list<int>::iterator it2 = word->getBooks()->begin(); it2 != word->getBooks()->end(); ++it2)
			{
				if (actualIdBook == *it2)
				{
					listOfPos = this->getListOfPositions(actualIdBook, actualIdTerm);
					repetition = listOfPos->size();
					delete listOfPos;

					//calcula el peso total del termino en el libro
					actualBookGlobalWeight += this->calculateBookGlobalWeight(actualIdBook, infinityNorm, totalBooks, numberDocsWithWord, repetition);
				}
			}
		}

		//hay que guardar ordenandolo por peso global
		insertado = false;
		bookGlobalWeight = new BookGlobalWeight(actualIdBook, actualBookGlobalWeight);
		//recorre y inserta ordernado
		if (orderDocs->size() > 0)
		{
			for (list<BookGlobalWeight*>::iterator it3 = orderDocs->begin(); it3 != orderDocs->end(); ++it3)
			{
				if(!insertado)
				{
					if ((*it3)->getGlobalWeight() > bookGlobalWeight->getGlobalWeight())
					{
						orderDocs->insert(it3, bookGlobalWeight);
						insertado = true;
					}
				}
			}
		}

		if (!insertado)
		{
			orderDocs->push_back(bookGlobalWeight);
			insertado = true;
		}
	}

	return orderDocs;
}

double Mediator::calculateBookGlobalWeight(int idBook, int infinityNorm, int totalBooks, int numberDocsWithWord, int repetition)
{
	//p(ti, dj) = ( aij * log(N/n) ) / norma infinito
	//ti: termino i
	//dj: documento j
	//aij; cant de apariciones del termino i en el documento j
	//N: cant. docuemtnos Totales
	//ni: cantidad de docuemtnos donde aparece el termino i

	double division = (double)totalBooks / (double)numberDocsWithWord;
	double logaritm = log(division);
	double result = (double)(repetition * logaritm) / (double)infinityNorm;

	return result;
}

int Mediator::getInfinityNorm(int idBook)
{
	//busca la norma infinito
	ByteString* bsId = new ByteString();
	bsId->insertLast(&idBook,sizeof(int));
	string clave = bsId->toString();
	delete bsId;

	Key* keyIdBook = new Key(clave);
	pair<Record*, BPlusTreeIterator*> searchResult = this->infinityNormIndex->search(*keyIdBook);
	int infinityNorm = searchResult.first->getData()->readAsInt(0);
	delete searchResult.first;
	delete searchResult.second;
	delete keyIdBook;

	return infinityNorm;
}

list<int>* Mediator::getListOfPositions(int idBook, int idTerm)
{
	//busca la lista de ocurrencias de un termino en un libro
	ByteString* bs = new ByteString();
	bs->insertLast(&idBook,sizeof(int));
	bs->insertLast(&idTerm,sizeof(int));
	string clave = bs->toString();
	delete bs;

	Key* k = new Key(clave);
	list<int>* listOfPos = this->ocurrenceTree->searchAllIds(*k);
	delete k;

	return listOfPos;
}

void Mediator::exportInvertedList(string pathname)
{
	this->indexWrapper->exportInvertedList(pathname);
}

void Mediator::exportToTXT(fileItem item,string filepath)
{
	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.export");
	switch (item) {
	case LISTAS:
		this->exportInvertedList(indexPath+filepath);break;
	case NORMA_INFINITO:
			this->infinityNormIndex->exportToPlainText(indexPath+filepath,false,false);break;
	case TERMINOS:
		this->vocabularyIndex->exportToPlainText(indexPath+filepath,false,true);break;
	case OCURRENCIA_POSICIONAL:
		this->ocurrenceTree->exportToPlainText(indexPath+filepath,false,false);break;
	}
}
