#include "DataBookManager.h"
#include "../common/Resource.h"


DataBookManager::DataBookManager(string fileNameVR,int maxKeySizeBPTree)
{
	this->fileVariableRecord = new FileVariableRecord(fileNameVR);
	this->fileFlagsEditorial = new FileFlags("Editorial");
	this->fileFlagsAutor = new FileFlags("Autor");
	this->fileFlagsTitulos = new FileFlags("Titulos");
	this->fileFlagsPalabras = new FileFlags("Palabras");

	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.index");
	this->principalIndex = new BPlusTree(indexPath + "indexPrincipal",maxKeySizeBPTree);
	//FIXME

	string incrementPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.control") + "IntAutoInc.inc";
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
	if(!this->fileVariableRecord->openFile()){
		this->fileVariableRecord->createFile();
		this->fileVariableRecord->openFile();
	}
}

DataBookManager::~DataBookManager()
{
	this->fileVariableRecord->closeFile();
	delete this->fileVariableRecord;
	delete this->fileFlagsEditorial;
	delete this->fileFlagsAutor;
	delete this->fileFlagsTitulos;
	delete this->fileFlagsPalabras;
	this->fileAutoIncInteger.seekp(0);
	int lastId = this->autoIncInteger;
	this->fileAutoIncInteger.write((char*)&lastId,sizeof(int));
	this->fileAutoIncInteger.close();
	delete this->principalIndex;
}

bool DataBookManager::addIDtoFileFlags(IndexWrapper::indexItem item,int ID)
{
	bool added = false;
	switch(item){
	case IndexWrapper::AUTOR :
								added = fileFlagsAutor->addID(ID);
								break;
	case IndexWrapper::EDITORIAL :
								added = fileFlagsEditorial->addID(ID);
								break;
	case IndexWrapper::TITULO :
								added = fileFlagsTitulos->addID(ID);
								break;
	case IndexWrapper::PALABRAS :
								added = fileFlagsPalabras->addID(ID);
								break;
	}
	return added;
}

bool DataBookManager::removeIDtoFileFlags(IndexWrapper::indexItem item,int ID)
{
	bool removed = false;
	switch(item){
	case IndexWrapper::AUTOR :
								removed = fileFlagsAutor->removeID(ID);
								break;
	case IndexWrapper::EDITORIAL :
								removed = fileFlagsEditorial->removeID(ID);
								break;
	case IndexWrapper::TITULO :
								removed = fileFlagsTitulos->removeID(ID);
								break;
	case IndexWrapper::PALABRAS :
								removed = fileFlagsPalabras->removeID(ID);
								break;
	}
	return removed;
}

List<int>* DataBookManager::getListToFileFlag(IndexWrapper::indexItem item)
{
	List<int>* list;
	switch(item){
	case IndexWrapper::AUTOR :
								list = fileFlagsAutor->getListID();
								break;
	case IndexWrapper::EDITORIAL :
								list = fileFlagsEditorial->getListID();
								break;
	case IndexWrapper::TITULO :
								list = fileFlagsTitulos->getListID();
								break;
	case IndexWrapper::PALABRAS :
								list = fileFlagsPalabras->getListID();
								break;
	}
	return list;
}

bool DataBookManager::addBook(ByteString byteString)
{
	bool saved = false;
	int possibleID = this->autoIncInteger + 1;

	Book book;
	book.Hidratate(byteString);
	book.setId(possibleID);

	int position = this->fileVariableRecord->addBook(book.Serialize());
	if(position > -1)
	{
		this->autoIncInteger++;
		ByteString idBook;
		idBook.insertLast(&possibleID, sizeof(int));
		Key* key = new Key(idBook.toString());
		ByteString offset;
		offset.insertLast(&position, sizeof(int));
		Record* recPrincipalInd = new Record(key,&offset);
		this->principalIndex->add(recPrincipalInd);
		addIDtoFileFlags(IndexWrapper::AUTOR,idBook.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::EDITORIAL,idBook.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::PALABRAS,idBook.readAsInt(0));
		addIDtoFileFlags(IndexWrapper::TITULO,idBook.readAsInt(0));
		saved = true;
	}
	return saved;
}

ByteString DataBookManager::getBook(int ID)
{
	ByteString recordString;
	ByteString byteString;
	byteString.insertLast(&ID,sizeof(int));
	string idString = byteString.toString();
	Key key(idString);
	pair<Record*, BPlusTreeIterator*> searchResult = this->principalIndex->search(key);
	if(searchResult.first != NULL)
	{
		int position = searchResult.first->getData()->readAsInt(0);
		if(position > -1)
		{
			recordString = this->fileVariableRecord->readBook(position);
		}

		//delete poddo
		delete searchResult.first;
	}

	//delete poddo
	if (searchResult.second != NULL)
		delete searchResult.second;

	return recordString;
}

bool DataBookManager::removeRecord(int ID)
{
	bool deleted = false;
	ByteString byteString;
	byteString.insertLast(&ID,sizeof(int));
	string idString = byteString.toString();
	Key key(idString);
	pair<Record*, BPlusTreeIterator*> searchResult = this->principalIndex->search(key);
	if(searchResult.first != NULL)
	{
		int position = searchResult.first->getData()->readAsInt(0);
		if(position > -1)
		{
			this->fileVariableRecord->deleteBook(position);
			this->principalIndex->removeKey(key);
			ByteString bs(key.toString());
			removeIDtoFileFlags(IndexWrapper::AUTOR,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::EDITORIAL,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::PALABRAS,bs.readAsInt(0));
			removeIDtoFileFlags(IndexWrapper::TITULO,bs.readAsInt(0));
			deleted = true;
		}
	}
	return deleted;
}

string DataBookManager::getlistOfTakenBooks()
{
	ByteString bs = this->fileVariableRecord->readNext();
	string out;
	char id[5];
	sprintf(id, "%4s", "Id");
	out.append(id);
	out.append("|");

	char author[20];
	sprintf(author, "%19s", "Autor");
	out.append(author);
	out.append("|");

	char editorial[20];
	sprintf(editorial, "%19s", "Editorial");
	out.append(editorial);
	out.append("|");

	char title[100];
	sprintf(title, "%99s", "Titulo");
	out.append(title);
	out.append("|");

	char words[9];
	sprintf(words, "%8s", "Palabras");
	out.append(words);
	out.append("|");

	out.append("\n");

	while(!bs.isEmpty())
	{

		Book book;
		book.Hidratate(bs);

		out.append(book.getBookFormatedText());

		bs = this->fileVariableRecord->readNext();
	}
	return out;
}


