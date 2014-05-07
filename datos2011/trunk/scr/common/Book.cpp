#include "Book.h"

using namespace std;

Book::Book() {
	this->author = "";
	this->editorial = "";
	this->title = "";
	this->text = "";
	this->id = -1;
}

Book::~Book() {
}

void Book::setAuthor(string author){
	this->author = author;
}

void Book::setEditorial(string editorial){
	this->editorial = editorial;
}

void Book::setId(int id){
	this->id = id;
}

void Book::setTitle(string title){
	this->title = title;
}

void Book::setText(string text){
	this->text = text;
}

void Book::setWordCount(int count){
	this->wordCount = count;
}
//  | id (int 4 bytes) | autorsize $1 (int 4 bytes) | autor ($1 bytes) |
//  | editorialsize    | editorial | titlesize | title | textsize |
//  | text  |
ByteString Book::Serialize(){
	ByteString bs;
	bs.insertLast(&(this->id), sizeof(int));
	size_t authorSize = this->author.size();
	bs.insertLast(&(authorSize), sizeof(size_t));
	bs.insertLast(this->author);
	size_t editorialSize = this->editorial.size();
	bs.insertLast(&(editorialSize), sizeof(size_t));
	bs.insertLast(this->editorial);
	size_t titleSize = this->title.size();
	bs.insertLast(&(titleSize), sizeof(size_t));
	bs.insertLast(this->title);
	size_t textSize = this->text.size();
	bs.insertLast(&(textSize), sizeof(size_t));
	bs.insertLast(this->text);
	bs.insertLast(&(this->wordCount), sizeof(int));
	return bs;
}


bool Book::Hidratate(ByteString & bookByte){
	int position = 0;

	// id
	this->id = bookByte.readAsInt(position);
	position += sizeof(int);

	//  author
	size_t authorSize = bookByte.readAsInt(position);
	position += sizeof(size_t);
	this->author = (bookByte.read(position, authorSize)).toString();
	position += authorSize;

	// editorial
	size_t editorialSize = bookByte.readAsInt(position);
	position += sizeof(size_t);
	this->editorial = (bookByte.read(position, editorialSize)).toString();
	position += editorialSize;

	// title
	size_t titleSize = bookByte.readAsInt(position);
	position += sizeof(size_t);
	this->title = (bookByte.read(position, titleSize)).toString();
	position += titleSize;

	// text
	size_t textSize = bookByte.readAsInt(position);
	position += sizeof(size_t);
	this->text = (bookByte.read(position, textSize)).toString();
	position += textSize;

	// wordcount
	this->wordCount = bookByte.readAsInt(position);

	return true;
}

string Book::getEditorial(){
	return this->editorial;
}



string Book::getText(){
	return this->text;
}



string Book::getAuthor(){
	return this->author;
}



string Book::getTitle(){
	return this->title;
}



int Book::getWordCount(){
	return this->wordCount;
}





int Book::getId(){
	return this->id;
}

string Book::getBookFormatedText(){

	string out = "";

	char id[5];
	sprintf(id, "%4s", Utility::intToString(this->getId()).c_str());
	out.append(id);
	out.append("|");

	char author[20];
	sprintf(author, "%19s", this->getAuthor().c_str());
	out.append(author);
	out.append("|");

	char editorial[20];
	sprintf(editorial, "%19s", this->getEditorial().c_str());
	out.append(editorial);
	out.append("|");

	char title[100];
	sprintf(title, "%99s", this->getTitle().c_str());
	out.append(title);
	out.append("|");

	char words[9];
	sprintf(words, "%8s", Utility::intToString(this->getWordCount()).c_str());
	out.append(words);
	out.append("|");

	out.append("\n");

	return out;
}

