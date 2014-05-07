#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <string>
#include <list>
#include <set>
#include <map>
#include "../common/Book.h"
#include "../common/Resource.h"
#include "../common/ByteString.h"
#include "../Utility/Utility.h"
#include "Term.h"
#include <fstream>

using namespace std;

class FileParser {
private:
	ifstream* file;
	list<string> knownEditorials;
	set<string> stopwords;
	set<string> words;
	int lines;
	Book* book;

	map<string, Term*> terms;

	void processFile(string path);
	bool findEditorial(string line);
	bool isStopword(string word);
	void setEditorials(string fileName);
	list<string> getEditorials(string fileName);
	void addTerm(string word, unsigned int position);
	void addWord(string word, unsigned int &position);


public:

	FileParser();
	virtual ~FileParser();

	bool parse(string path);

	set<string> getWords();
	void setWords(string text);
	int getWordCount();
	Book* getBook();
	void addEditorial(string editorial);
	unsigned int getInfinityNorm();

	void MostrarTeminos();
    map<string,Term*> getTerms() const;
    void setTerms(map<string,Term*> terms);
};

#endif /* FILEPARSER_H_ */
