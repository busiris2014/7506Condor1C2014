#include "FileParser.h"

FileParser::FileParser()
{
	this->lines = Utility::stringToInt(Resource::getConfigProperties()->get("booquerio.editorial.cantlineas"));
	string controlPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.control");
	this->knownEditorials = getEditorials(controlPath + "editoriales.config");
	this->stopwords = Utility::splitToSet(Resource::getConfigProperties()->get("booquerio.stopwords"), ";");
	book = new Book();

	filepos = 0;
	lastline.clear();
}

FileParser::~FileParser() {
	string controlPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.control");
	setEditorials(controlPath + "editoriales.config");

	delete(book);

	for(map<string,Term*>::iterator it = this->terms.begin(); it != this->terms.end(); ++it)
		delete (*it).second;
}


bool FileParser::parse(string path)
{
	string fileName;
	string authorData;
	string type;

	bool validPath = true;
	bool end = false;
	int index = -1;

	// Se intenta abrir el archivo
	this->file = new ifstream(path.c_str());

	if (!*this->file)
	{
		delete(this->file);
		validPath = false;
	}

	if (validPath)
	{
		/* Se obtiene solo el nombre del archivo que contiene autor - titulo  */
		fileName = Utility::normalizeString(path);
		while(!end)
		{
			index = fileName.find('/', 0);
			if (index >= 0)
				fileName = fileName.substr((fileName.find('/', 0)+1), fileName.length());
			else
				end = true;
		}

		/* Del comienzo hasta la primer "." es el AUTOR */
		authorData = fileName.substr(0, fileName.length()-4);

		type = fileName.substr(fileName.length()-3, fileName.length());
		type = Utility::toLower(type);

		book->setAuthor(Utility::trim(authorData));


		if(type.compare("twt")==0 ){
			/* Se procesa el archivo */
			this->processTWT(path);
			book->setTitle("T");
		} else if(type.compare("rss")==0 ){
			this->processRSS(path);
			book->setTitle("N");
		} else {
			delete(this->file);
			validPath = false;
		}

		delete(this->file);
	}

	return (validPath & end);
}



ByteString FileParser::processDescription(ByteString input){
	int index = 0;
	ByteString output;
	string word;
	size_t end;

	while (end == string::npos){
		end = input.toString().find(' ',index);
		if(end == string::npos)
			word = input.toString().substr(index,input.toString().length());
		else
			word = input.toString().substr(index,end);

		if(!(isStopword(word)))
    		output.insertLast(Utility::toLower(word));

		index =  input.toString().find(' ',index)+1;
	}

	return input;
}


void FileParser::processTWT(string path)
{
	ByteString text;
	string titleData;
	string dateData;
	string textData;

	string line;
	bool endFile = false;

	this->file = new ifstream(path.c_str());

	this->file->seekg(filepos);

	getline(*this->file, line);

	if (this->file->eof() || this->file->bad())
		endFile = true;

	if (!endFile)
	{
		dateData = line.substr(0,(line.find('@', 0)-1));
		dateData = Utility::dateFormat(dateData);
		book->setWordCount(Utility::stringToInt(dateData));

		textData = line.substr(line.find("says: ")+6,line.length());
		text.clean();
		text.insertLast(Utility::trim(textData));
		book->setText(processDescription(text).toString());

		if (line.find('#', 0) != string::npos){
			titleData = line.substr(line.find('#', 0)+1,line.length());
			titleData = titleData.substr(0,titleData.find(' ', 0));
			book->setEditorial(titleData);
		}else{
			book->setEditorial("n/a");
		}

	} //else {
	//	delete this->book;
	//	this->book = NULL;
	//}

	filepos = this->file->tellg();

}



void FileParser::processRSS(string path)
{
	ByteString text;
	string authorData;
	string titleData;
	string dateData;
	string textData;

	string line;
	bool endFile = false;
	bool rssend = false;
	bool addingDescription = false;

	this->file = new ifstream(path.c_str());

	this->file->seekg(filepos);

	while(!endFile && !rssend)
	{
		if(this->lastline.empty())
			getline(*this->file, line);
		else
			{
			line = this->lastline;
			lastline.clear();
			}

		if (this->file->eof() || this->file->bad())
			endFile = true;

		if (!endFile)
		{
			if(line.find("Title: ") != string::npos)
				{
				if(!rssend)
					{
					titleData = line.substr(7,line.length());
					book->setEditorial(Utility::trim(titleData));
					}
				else
					{
					book->setText(processDescription(text).toString());//termina el rss anterior
					filepos = this->file->tellg();
					this->lastline = line;
					rssend = true;
					}
				}

			else if(line.find("Author: ") != string::npos)
				{
				authorData = line.substr(8,line.length());
				book->setAuthor(Utility::trim(authorData));
				}

			else if(line.find("Description: ") != string::npos)
				{
				text.clean();
				textData = line.substr(13,line.length());
				addingDescription = true;
				}

			else if(line.find("Pub.date: ") != string::npos)
				{
				addingDescription = false;

				dateData = line.substr(10,line.length());
				dateData = Utility::dateFormat(Utility::trim(dateData));
				book->setWordCount(Utility::stringToInt(dateData));
				}
			else
				{
				if(addingDescription)
					text.insertLast(Utility::trim(line));
				}
		}

	}
}


bool FileParser::findEditorial(string line)
{
	list<string>::iterator it;
	string normalizedLine;
	string editorial;
	bool findIt = false;

	normalizedLine = Utility::normalizeString(line);

	for(it = this->knownEditorials.begin(); it != this->knownEditorials.end(); ++it)
	{
		editorial = (*it);
		editorial = Utility::normalizeString(editorial);

		if (normalizedLine.find(editorial) != string::npos)
		{
			book->setEditorial((*it));
			findIt = true;
			break;
		}
	}
	return findIt;
}

void FileParser::setWords(string text)
{
	this->words.clear();
	this->terms.clear();
	int initial = 0; // indice que apunta al primer caracter valido
	int final = 0;   // indice que apunta al primer caracter invalido
	char chr;
	string word;
	unsigned int index = 0;
	unsigned int position = 0;

	while (index < text.size())
	{
		chr = text[index];

		if (chr == '\303')
		{
			// Caso particular para un caracter extendido --> tienen dos caracteres relacionados
			// Si lo es    --> se toma como parte de la palabra
			// Si no lo es --> se ignora y se continua con el proximo caracter
			char nextChar = text[index + 1];

			if (Utility::getNormalizedChar(nextChar) != 0x00)
			{
				index += 2;
				final += 2;
			}
		}
		else
		{
			if ((chr >= (char)65 && chr <= (char)90) || (chr >= (char)97 && chr <= (char)122) || (chr >= (char)48 && chr <= (char)57))
			{
				// El caracter es una letra o un numero
				final += 1;
			}
			else
			{
				// Se corta la secuencia y se toma la palabra
				word = text.substr(initial, final - initial);

				this->addWord(word, position);
//
//				if (!Utility::empty(word))
//				{
//					// Se actualiza la posicion de la palabra en el texto
//					position += 1;
//
//					// Se normaliza la palabra
//					word = Utility::normalizeString(word);
//
//					// Se incorpora a la lista de palabras, si no es stopword y si ya no existe en la lista
//					if(!(this->isStopword(word)) && !(this->words.count(word) > 0))
//					{
//						this->words.insert(word);
//					}
//
//					// ----------------------------------- //
//					// Se incorpora a la lista de terminos
//					// ----------------------------------- //
//					if(!(this->isStopword(word)))
//						this->addTerm(word, position);
//					// ----------------------------------- //
//				}

				initial = index + 1;

				final = initial;
			}

			index += 1;
		}
	}

	// Se incorpora la ultima palabra
	word = text.substr(initial, final - initial);
	this->addWord(word, position);

	//this->MostrarTeminos();
}

bool FileParser::isStopword(string word)
{
	return (this->stopwords.count(word) > 0);
}

set<string> FileParser::getWords()
{
	return this->words;
}

int FileParser::getWordCount()
{
	return this->words.size();
}

Book* FileParser::getBook()
{
	return this->book;
}


list<string> FileParser::getEditorials(string fileName)
{
	ifstream file;
	file.open(fileName.c_str(), ios::in | ios::out | ios::binary);
	string editorials;
	getline(file,editorials);
	file.close();
	return Utility::split(editorials,";");
}

void FileParser::setEditorials(string fileName)
{
	list<string>::iterator it;
	string editorial;
	ofstream file;

	remove(fileName.c_str());

	//FILE* result = fopen(fileName.c_str(),"w+");

	file.open(fileName.c_str(), ios::in | ios::out | ios::binary);
	if(file.is_open())
	{
		file.seekp(0);
		string editorials;
		for(it = this->knownEditorials.begin(); it != this->knownEditorials.end(); ++it)
		{
			editorial = (*it);
			editorial += ";";
			editorials += editorial;
		}
		file<<editorials<<endl;
		file.close();
	}
}

void FileParser::addEditorial(string editorial)
{
	list<string>::iterator it;
	bool findIt;
	editorial = Utility::normalizeString(editorial);
	string actualEditorial;
	for(it = this->knownEditorials.begin(); it != this->knownEditorials.end(); ++it)
	{
		actualEditorial = (*it);
		actualEditorial = Utility::normalizeString(actualEditorial);

		if (strcmp(actualEditorial.c_str(),editorial.c_str()) == 0)
		{
			findIt = true;
			break;
		}
	}
	if(!findIt)
		this->knownEditorials.push_back(editorial);
}

void FileParser::addWord(string word, unsigned int &position)
{
	if (!Utility::empty(word))
	{
		// Se actualiza la posicion de la palabra en el texto
		position += 1;

		// Se normaliza la palabra
		word = Utility::normalizeString(word);

		// Se incorpora a la lista de palabras, si no es stopword y si ya no existe en la lista
		if(!(this->isStopword(word)) && !(this->words.count(word) > 0))
		{
			this->words.insert(word);
		}
		// ----------------------------------- //
		// Se incorpora a la lista de terminos
		// ----------------------------------- //
		if(!(this->isStopword(word)))
			this->addTerm(word, position);
		// ----------------------------------- //
	}
}


void FileParser::addTerm(string word, unsigned int position)
{
	if (this->terms.count(word) == 0)
	{
		Term* newTerm = new Term(word, position);
		this->terms.insert(pair<string,Term*>(word, newTerm));
	}
	else
	{
		map<string,Term*>::iterator it = this->terms.find(word);

		if (it != this->terms.end())
		{
			//Term* term = it->second;
			//term->addPosition(position);
			it->second->addPosition(position);
		}
	}
}

unsigned int FileParser::getInfinityNorm()
{
	unsigned int maximumFrequency = 0;

	for(map<string,Term*>::iterator it = this->terms.begin(); it != this->terms.end(); ++it)
	{
		if (maximumFrequency < (*it).second->getFrecuency())
			maximumFrequency = (*it).second->getFrecuency();
	}

	return maximumFrequency;
}

map<string,Term*> FileParser::getTerms() const
{
    return terms;
}

void FileParser::setTerms(map<string,Term*> terms)
{
    this->terms = terms;
}

void FileParser::MostrarTeminos()
{
	// Se recorre todos los terminos del diccionario
	for(map<string,Term*>::iterator it = this->terms.begin(); it != this->terms.end(); ++it)
	{
		cout << "Palabra: '" << (*it).second->getWord() << "'" << endl;
		cout << "Frecuencia: " << (*it).second->getFrecuency() << " apariciones" << endl;
		cout << "Posiciones: " << endl;

		list<unsigned int> positions = (*it).second->getPositions();

		// Se recorre todas las posiciones de la lista
		for (list<unsigned int>::iterator it2 = positions.begin(); it2 != positions.end(); ++it2)
		{
			cout << (*it2) << endl;
		}
		cout << endl;
	}

	cout << "Norma Infinito: '" << this->getInfinityNorm() << "'" << endl;
}
