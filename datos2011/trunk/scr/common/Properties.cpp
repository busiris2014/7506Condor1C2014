#include "Properties.h"

Properties::Properties() {
}

Properties::Properties(string fileName)
{
	this->properties = NULL;
	this->fileName = "";

	this->setFileName(fileName);
}

Properties::~Properties()
{
	if (this->file != NULL)
		delete(this->file);

	if (this->properties != NULL)
	{
		this->properties->erase(this->properties->begin(), this->properties->end());
		this->properties->clear();
		delete (this->properties);
	}
}

void Properties::setFileName(string fileName)
{
	if (this->fileName.empty())
	{
		// Se setea el nombre del archivo
		this->fileName = fileName;

		// Se prueba abrir el archivo
		this->file = new ifstream(this->fileName.c_str());

		if (!*this->file)
		{
			delete(this->file);
			this->file = NULL;
		}

		delete(this->file);
		this->file = NULL;
	}
}

string Properties::get(string key)
{
	string result = "";

	if (this->properties == NULL)
		this->initialize();

	map<string,string>::iterator it = this->properties->find(key);

	if (it != this->properties->end())
		result = it->second;

	return result;
}

int Properties::getAsInt(string key)
{
	string result = Properties::get(key);
	return Utility::stringToInt(result);
}

char Properties::getAsChar(string key)
{
	string result = Properties::get(key);
	char charResult = 0;

	if (result.length()>0)
		charResult = (char)Utility::stringToInt(result.substr(0,1));

	return charResult;
}

void Properties::initialize()
{
	this->properties = new map<string,string>();
	this->file = new ifstream(this->fileName.c_str());
	bool endFile = false;

	while (!endFile)
	{
		string line;
		getline(*this->file,line);

		if (this->file->eof())
			endFile = true;

		if (this->file->bad())
			endFile = true;

		this->processLine(line);
	}

	this->file->close();
	delete(this->file);
	this->file = NULL;
}

void Properties::processLine(string line)
{
	bool comment = false;
	unsigned int indexComment;
	unsigned int index;

	indexComment = line.find_first_of(PROP_COMENTARIO);

	if (indexComment != string::npos)
	{
		string previous = line.substr(0, indexComment);
		previous.erase(indexComment);

		if (Utility::empty(previous))
			comment = true;
	}

	if (!comment)
	{
		index = line.find_first_of(PROP_SEPARADOR);

		// si la linea no contiene el signo igual se descarta
		if (index != string::npos)
		{
			string key = line.substr(0, index);
			key.erase(index);
			key = Utility::trim(key);

			string value = line.substr(index + string(PROP_SEPARADOR).size());
			value = Utility::trim(value);

			this->properties->insert(pair<string,string>(key,value));
		}
	}
}
