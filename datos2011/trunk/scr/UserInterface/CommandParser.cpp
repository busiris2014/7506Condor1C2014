#include "CommandParser.h"

CommandParser::CommandParser()
{
	this->option = "";
	this->fileId = "";
	this->pathFile = "";
	this->queryTerm = "";
	this->exportIndexStructure = false;
}

CommandParser::~CommandParser()
{
}

bool CommandParser::validArguments(int count, char** params)
{
	string option;
	bool valid = false;
	int i;

	if (count > 1)
	{
		// Se obtiene la opcion elegida
		option = params[1];

		if ((Utility::equals(option, TOMAR_TEXTO)||(this->isQueryArguments(option))) && (count >= 3))
		{
			string secondParameter = "";

			for(i=2; i<count; i+=1)
			{
				secondParameter = Utility::concat(secondParameter, params[i]);
				secondParameter = Utility::concat(secondParameter, SPACE);
			}

			if (Utility::equals(option, TOMAR_TEXTO))
				this->pathFile = Utility::trim(secondParameter);
			else
				this->queryTerm = Utility::trim(secondParameter);

			valid = true;
		}
		else if ((this->isIndexArguments(option) || Utility::equals(option, LISTAR_ARCHIVOS)) && (count == 2))
			valid = true;
		else if ((Utility::equals(option, OBTENER_ARCHIVO) ||
				Utility::equals(option, QUITAR_ARCHIVO)) && (count == 3))
		{
			// Se carga el ID del archivo
			this->fileId = params[2];
			valid = true;
		}
		else if (Utility::equals(option, VER_ESTRUCTURA) && (count >= 4))
		{
			if (this->isIndexArguments(params[2]))
			{
				valid = true;
				this->exportIndexStructure = true;

				if(Utility::equals(params[2], PROCESAR_EDITORIAL))
					this->indexId = IndexWrapper::EDITORIAL;
				if (Utility::equals(params[2], PROCESAR_AUTOR))
					this->indexId = IndexWrapper::AUTOR;
				if (Utility::equals(params[2], PROCESAR_TITULO))
					this->indexId = IndexWrapper::TITULO;
				if (Utility::equals(params[2], PROCESAR_PALABRAS))
					this->indexId = IndexWrapper::PALABRAS;
			}
			else if (this->isStructureArguments(params[2]))
			{
				valid = true;
				if (Utility::equals(params[2], ARCHIVO_TERMINOS))
					this->IdFile = Mediator::TERMINOS;
				if (Utility::equals(params[2], ARCHIVO_NORMA_INFINITO))
					this->IdFile = Mediator::NORMA_INFINITO;
				if (Utility::equals(params[2], ARCHIVO_OCURRENCIA_POSICIONAL))
					this->IdFile = Mediator::OCURRENCIA_POSICIONAL;
				if (Utility::equals(params[2], LISTAS_INVERTIDAS))
					this->IdFile = Mediator::LISTAS;
			}

			if (valid)
			{
				for(i=3; i<count; i+=1)
				{
					this->pathFile = Utility::concat(this->pathFile, params[i]);
					this->pathFile = Utility::concat(this->pathFile, SPACE);
				}

				this->pathFile = Utility::trim(this->pathFile);
			}
		}
		else if (Utility::equals(option, AYUDA) && (count == 2))
		{
				valid = true;
		}
	}

	if (valid)
		this->option = option;

	return valid;
}

string CommandParser::printHelp()
{
	string help;
	help = '\n';
	help += "************************************************************\n";
	help += "		Parametros Aplicacion Booquerio\n";
	help += "************************************************************\n";
	help += "Tomar Archivo:       '-i archivo.txt'\n";
	help += "Procesar Editorial:  '-e'\n";
	help += "Procesar Autor:      '-a'\n";
	help += "Procesar Titulo:     '-t'\n";
	help += "Procesar Palabra:    '-p'\n";
	help += "Listar Archivo:      '-l'\n";
	help += "Obtener Archivo:     '-o idArchivo'\n";
	help += "Quitar Archivo:      '-q idArchivo'\n";
	help += "Consultar Editorial: '-qe editorial'\n";
	help += "Consultar Autor:     '-qa autor'\n";
	help += "Consultar Titulo:    '-qt titulo'\n";
	help += "Consultar Palabras:  '-qp palabras para busqueda por cercania y rankeada'\n";
	help += "Ver Estructura:      '-v [-e|-a|-t|-p|-at|-ani|-aop|-li] nombreArchivoSalida'\n";
	help += "Ayuda:               '-h'\n";
	help += '\n';
	return help;
}

bool CommandParser::isIndexArguments(string option)
{
	return (Utility::equals(option, PROCESAR_EDITORIAL)||
			Utility::equals(option, PROCESAR_AUTOR)||
			Utility::equals(option, PROCESAR_TITULO)||
			Utility::equals(option, PROCESAR_PALABRAS));
}

bool CommandParser::isQueryArguments(string option)
{
	return (Utility::equals(option, CONSULTAR_EDITORIAL)||
			Utility::equals(option, CONSULTAR_AUTOR)||
			Utility::equals(option, CONSULTAR_TITULO)||
			Utility::equals(option, CONSULTAR_PALABRAS));
}

bool CommandParser::isStructureArguments(string option)
{
	return (Utility::equals(option, ARCHIVO_TERMINOS)||
			Utility::equals(option, ARCHIVO_NORMA_INFINITO)||
			Utility::equals(option, ARCHIVO_OCURRENCIA_POSICIONAL)||
			Utility::equals(option, LISTAS_INVERTIDAS));
}

CommandOption CommandParser::getOption()
{
	CommandOption enumOption;

	if (Utility::equals(option, TOMAR_TEXTO))
		enumOption = TomarTexto;
	else if (Utility::equals(option, PROCESAR_EDITORIAL))
		enumOption = ProcesarEditorial;
	else if (Utility::equals(option, PROCESAR_AUTOR))
		enumOption = ProcesarAutor;
	else if (Utility::equals(option, PROCESAR_TITULO))
		enumOption = ProcesarTitulo;
	else if (Utility::equals(option, PROCESAR_PALABRAS))
		enumOption = ProcesarPalabras;
	else if (Utility::equals(option, LISTAR_ARCHIVOS))
		enumOption = ListarArchivos;
	else if (Utility::equals(option, OBTENER_ARCHIVO))
		enumOption = ObtenerArchivo;
	else if (Utility::equals(option, QUITAR_ARCHIVO))
		enumOption = QuitarArchivo;
	else if (Utility::equals(option, CONSULTAR_EDITORIAL))
		enumOption = ConsultarEditorial;
	else if (Utility::equals(option, CONSULTAR_AUTOR))
		enumOption = ConsultarAutor;
	else if (Utility::equals(option, CONSULTAR_TITULO))
		enumOption = ConsultarTitulo;
	else if (Utility::equals(option, CONSULTAR_PALABRAS))
		enumOption = ConsultarPalabras;
	else if (Utility::equals(option, VER_ESTRUCTURA))
	{
		if (this->exportIndexStructure)
			enumOption = VerEstructuraIndices;
		else
			enumOption = VerEstructuraArchivos;
	}
	else if (Utility::equals(option, AYUDA))
		enumOption = Ayuda;

	return enumOption;
}

string CommandParser::getPathFile()
{
	return this->pathFile;
}

string CommandParser::getFileId()
{
	return this->fileId;
}

string CommandParser::getQueryTerm()
{
	return this->queryTerm;
}

IndexWrapper::indexItem CommandParser::getIndexId()
{
	return this->indexId;
}

Mediator::fileItem CommandParser::getIdFile()
{
	return this->IdFile;
}
