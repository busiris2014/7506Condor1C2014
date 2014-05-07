#include "UserInterface/CommandParser.h"
#include "parser/FileParser.h"
#include "UserInterface/Mediator.h"
#include "Utility/Utility.h"
#include <iostream>

using namespace std;

bool existsConfigurationFile(string path)
{
	ifstream* file;
	bool exists = true;

	file = new ifstream(path.c_str());

	if (!*file)
		exists = false;

	delete(file);

	return exists;
}

string getIndexInternalName(IndexWrapper::indexItem indexId){
	switch(indexId){
	case IndexWrapper::AUTOR:
		return "IndiceAutor";
	case IndexWrapper::EDITORIAL:
		return "IndiceEditorial";
	case IndexWrapper::TITULO:
		return "IndiceTitulo";
	case IndexWrapper::PALABRAS:
		return "IndicePalabras";
	}
}

void takeText(string path)
{
	Mediator* mediator = new Mediator();
	Book* book;
	string editorial;
	book = mediator->getBook(path);
	if (book != NULL)
	{
		if (Utility::empty(book->getEditorial()))
		{
			cout << "La editorial del libro no ha podido ser encontrada." << endl;
			cout << "Ingrese la editorial: ";
			//cin >> editorial;
			getline(cin, editorial);

			book->setEditorial(editorial);

			if (!Utility::empty(editorial))
			{
				mediator->addEditorial(editorial);
			}

		}

		if (mediator->addBook(book) == false)
			cout << "No se ha podido agregar el libro al sistema." << endl;
		else
			cout << "El libro ha sido incorporado al sistema en forma exitosa!" << endl;
	}
	else
		cout << "No se ha podido parsear el archivo ingresado." << endl;

	delete mediator;
}

void processEditorial()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::EDITORIAL, "IndiceEditorial", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::EDITORIAL))
		cout << "No se pudo indexar por Editorial." << endl;

	delete mediator;
}

void processAuthor()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::AUTOR, "IndiceAutor", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::AUTOR))
		cout << "No se pudo indexar por Autor." << endl;

	delete mediator;
}

void processTitle()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::TITULO, "IndiceTitulo", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::TITULO))
		cout << "No se pudo indexar por Titulo." << endl;
	mediator->closeIndex(IndexWrapper::TITULO);
	delete mediator;
}

void processWords()
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	if (!mediator->indexingListOnIndex(IndexWrapper::PALABRAS))
		cout << "No se pudo indexar por Palabras." << endl;
	mediator->closeIndex(IndexWrapper::PALABRAS);
	delete mediator;
}

void listFiles()
{
	Mediator* mediator = new Mediator();
	string listOfTakenBooks = mediator->getListOfTakenBooks();
	cout<<endl;
	cout<<listOfTakenBooks<<endl;
	delete mediator;
}

void getFiles(string fileId)
{
	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.export");
	Mediator* mediator = new Mediator();
	int id = Utility::stringToInt(fileId);
	ByteString bs = mediator->getBookFromFile(id);
	if(!bs.isEmpty())
	{
		Book book;
		book.Hidratate(bs);
		string fileName = indexPath + book.getAuthor() + " - " + book.getTitle() + ".txt";
		FILE* result = fopen(fileName.c_str(),"w+");
		if(result)
		{
			ofstream file;
			file.open(fileName.c_str(), ios::in | ios::out | ios::binary);
			if(file.is_open())
			{
				file.seekp(0);
				string text = book.getText();
				file.write(text.c_str(),text.size());
				cout << "Archivo exportado satisfactoriamente a: " << fileName << endl;
			}
		}
	}
	else
		cout << "El libro solicitado no existe"<< endl;

	delete mediator;
}

void deleteFiles(string fileId)
{
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::AUTOR, "IndiceAutor", 50);
	mediator->initializeIndex(IndexWrapper::EDITORIAL, "IndiceEditorial", 50);
	mediator->initializeIndex(IndexWrapper::TITULO, "IndiceTitulo", 50);
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);

	int id = Utility::stringToInt(fileId);
	bool deleted = mediator->removeBook(id);
	if(deleted)
		cout<<"Se ha borrado el archivo Exitosamente"<<endl;
	else
		cout<<"No se ha podido borrar el archivo"<<endl;

	delete mediator;
}

void seeStructure(IndexWrapper::indexItem indexId, string pathFile){

	Mediator* m = new Mediator();
	m->initializeIndex(indexId, getIndexInternalName(indexId), 50);
	m->exportToPlainText(indexId, pathFile);
	cout << "Archivo exportado satisfactoriamente a: " << pathFile << endl;
	delete m;
}

void queryEditorial(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::EDITORIAL, getIndexInternalName(IndexWrapper::EDITORIAL), 50);
	cout << m->search(IndexWrapper::EDITORIAL, toSearch) << endl;
	m->closeIndex(IndexWrapper::EDITORIAL);
	delete m;
}

void queryAuthor(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::AUTOR, getIndexInternalName(IndexWrapper::AUTOR), 50);
	cout << m->search(IndexWrapper::AUTOR, toSearch) << endl;
	m->closeIndex(IndexWrapper::AUTOR);
	delete m;
}

void queryTitle(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::TITULO, getIndexInternalName(IndexWrapper::TITULO), 50);
	cout << m->search(IndexWrapper::TITULO, toSearch) << endl;
	m->closeIndex(IndexWrapper::TITULO);
	delete m;
}

void queryTerms(string toSearch){
	Mediator* m = new Mediator();
	m->initializeIndex(IndexWrapper::PALABRAS, getIndexInternalName(IndexWrapper::PALABRAS), 50);
	cout << m->search(IndexWrapper::PALABRAS, toSearch) << endl;
	m->closeIndex(IndexWrapper::PALABRAS);
	delete m;
}

void seeStructureFile(Mediator::fileItem IdFile, string pathFile)
{
	Mediator* m = new Mediator();
	Mediator* mediator = new Mediator();
	mediator->initializeIndex(IndexWrapper::PALABRAS, "IndicePalabras", 50);
	m->exportToTXT(IdFile,pathFile);
	delete m;
}

void printHelp(string msg)
{
	cout << msg;
}


int main(int argc, char** argv)
{
	//Se guarda el application path en la variable static
	Utility::setApplicationPath(argv[0]);

	if (existsConfigurationFile(Utility::aplicationPath + "booquerio.config"))
	{
		CommandParser* commandParser = new CommandParser();
		if (commandParser->validArguments(argc, argv))
		{

			switch (commandParser->getOption())
			{

				case TomarTexto:
					takeText(commandParser->getPathFile());
					break;
				case ProcesarEditorial:
					processEditorial();
					break;
				case ProcesarAutor:
					processAuthor();
					break;
				case ProcesarTitulo:
					processTitle();
					break;
				case ProcesarPalabras:
					processWords();
					break;
				case ListarArchivos:
					listFiles();
					break;
				case ObtenerArchivo:
					getFiles(commandParser->getFileId());
					break;
				case QuitarArchivo:
					deleteFiles(commandParser->getFileId());
					break;
				case VerEstructuraIndices:
					seeStructure(commandParser->getIndexId(), commandParser->getPathFile());
					break;

				case VerEstructuraArchivos:
					cout << "Opcion: Ver Estructura Archivos" << endl;
					cout << "Archivo Salida: '" << commandParser->getPathFile() << "'" << endl;
					seeStructureFile(commandParser->getIdFile(), commandParser->getPathFile());
					break;
				case ConsultarEditorial:
					cout << "Opcion: Consultar Editorial" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryEditorial(commandParser->getQueryTerm());
					break;
				case ConsultarAutor:
					cout << "Opcion: Consultar Autor" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryAuthor(commandParser->getQueryTerm());
					break;
				case ConsultarTitulo:
					cout << "Opcion: Consultar Titulo" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryTitle(commandParser->getQueryTerm());
					break;
				case ConsultarPalabras:
					cout << "Opcion: Consultar Palabras" << endl; //commandParser->getOption();
					cout << "Termino de Consulta: '" << commandParser->getQueryTerm() << "'" << endl;
					queryTerms(commandParser->getQueryTerm());
					break;

				case Ayuda:
					printHelp(commandParser->printHelp());
					break;
			};

		}
		else
			cout << "Los argumentos son invalidos" << endl;

		delete commandParser;
	}
	else
		cout << "No se ha podido ejecutar la aplicacion: no se encontró el archivo de configuracion 'booquiero.config'" << endl;

	return 0;
}


