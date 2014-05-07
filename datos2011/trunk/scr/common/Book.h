#ifndef BOOK_H_
#define BOOK_H_

#include "../common/ByteString.h"
#include "../common/Serializer.h"
#include "../Utility/Utility.h"

using namespace std;

class Book : public Serializer{
private:
	string author;
	string title;
	string editorial;
	string text;
	int id;
	int wordCount;

public:
	Book();
	virtual ~Book();

	void setAuthor(string author);
	void setTitle(string title);
	void setEditorial(string editorial);
	void setText(string text);
	void setWordCount(int count);
	void setId(int id);

	//Devuelve el libro serializada
	ByteString Serialize();

	//Carga el objeto a partir de uno serializado
	bool Hidratate(ByteString & bookByte);

	string getAuthor();
	string getTitle();
	string getEditorial();
	string getText();
	string getBookFormatedText();
	int getWordCount();
	int getId();
};

#endif /* BOOK_H_ */
