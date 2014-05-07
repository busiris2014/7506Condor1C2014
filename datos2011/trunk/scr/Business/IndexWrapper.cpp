#include "IndexWrapper.h"
#include "../common/Resource.h"

using namespace std;

void IndexWrapper::createIndex(string path, indexItem itemType, int keyTopSize) {

	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.index");
	switch (itemType) {
	case IndexWrapper::AUTOR:
		this->author = new ClassifBPlusTree(indexPath + path, keyTopSize);
		break;
	case IndexWrapper::EDITORIAL:
		this->editorial = new ClassifBPlusTree(indexPath + path, keyTopSize);
		break;
	case IndexWrapper::PALABRAS:
		this->words = new HashClasification(indexPath + path);
		break;
	case IndexWrapper::TITULO:
		this->tittle = new HashSelection(indexPath + path);
		break;
	default:
		throw "Not an item handled";
	}

}

void IndexWrapper::closeIndex(indexItem itemType) {

	switch (itemType) {
	case IndexWrapper::AUTOR:
		if(this->author != NULL)
			delete this->author;
		break;
	case IndexWrapper::EDITORIAL:
		if(this->editorial != NULL)
			delete this->editorial;
		break;
	case IndexWrapper::PALABRAS:
		if(this->words != NULL)
			delete this->words;
		break;
	case IndexWrapper::TITULO:
		if(this->tittle != NULL)
			delete this->tittle;
		break;
	default:
		throw "Not an item handled";
	}

}

bool IndexWrapper::add(Record* r, indexItem itemType) {

	switch (itemType) {
	case IndexWrapper::AUTOR:
		return this->author->add(r);
	case IndexWrapper::EDITORIAL:
		return this->editorial->add(r);
	case IndexWrapper::PALABRAS:
		return this->words->insertRecord(r);
	case IndexWrapper::TITULO:
		return this->tittle->insertRecord(r);
	default:
		throw "Not an item handled";
	}

}

bool IndexWrapper::remove(Record* r, indexItem itemType) {

	switch (itemType) {
	case IndexWrapper::AUTOR:
		return this->author->removeKey(*(r->getKey()), r->getData()->readAsInt(0));
	case IndexWrapper::EDITORIAL:
		return this->editorial->removeKey(*(r->getKey()), r->getData()->readAsInt(0));
	case IndexWrapper::PALABRAS:
		return this->words->deleteRecord(r);
	case IndexWrapper::TITULO:
		return this->tittle->deleteRecord(r);
	default:
		throw "Not an item handled";
	}
}

Record* IndexWrapper::search(Key* key, indexItem itemType) {

	switch (itemType) {
	case IndexWrapper::AUTOR:
		return this->author->search(*(key)).first;
	case IndexWrapper::EDITORIAL:
		return this->editorial->search(*(key)).first;
	case IndexWrapper::PALABRAS:
		return this->words->findRecord(key);
	case IndexWrapper::TITULO:
		return this->tittle->findRecord(key);
	default:
		throw "Not an item handled";
	}
}

list<int>* IndexWrapper::searchAllIds(Key* k, indexItem itemType){
	switch (itemType) {
		case IndexWrapper::AUTOR:
			return this->author->searchAllIds(*k);
		case IndexWrapper::EDITORIAL:
			return this->editorial->searchAllIds(*k);
		case IndexWrapper::PALABRAS:
			return this->words->searchRecord(k);
		case IndexWrapper::TITULO:
			return this->tittle->searchRecord(k);
		default:
			throw "Not an item handled";
		}
}

IndexWrapper::~IndexWrapper(){
	if(this->author != NULL)
		delete(this->author);
	if(this->editorial != NULL)
		delete(this->editorial);
	if(this->tittle != NULL)
		delete(this->tittle);
	if(this->words != NULL)
		delete(this->words);

}


bool IndexWrapper::removeFromAll(Book b){
	bool toReturn = true;
	Record r;
	int bookId = b.getId();
	r.setData(new ByteString(&bookId, sizeof(int)));
	// author
	if (this->author != NULL) {
		r.setKey(new Key(b.getAuthor()));
		if(!this->remove(&r, IndexWrapper::AUTOR))
			toReturn = false;
	}
	// editorial
	if (this->editorial != NULL) {
		r.setKey(new Key(b.getEditorial()));
		if(!this->remove(&r, IndexWrapper::EDITORIAL))
			toReturn = false;
	}
	// title
	if (this->tittle != NULL) {
		r.setKey(new Key(b.getTitle()));
		if(!this->remove(&r, IndexWrapper::TITULO))
			toReturn = false;
	}
	// words
	if (this->words != NULL) {
		FileParser parser;
		parser.setWords(b.getText());
		set<string> words = parser.getWords();
		for(set<string>::iterator it = words.begin(); it != words.end(); ++it){
			r.setKey(new Key(*it));
			if(!this->remove(&r, IndexWrapper::PALABRAS))
				toReturn = false;
		}

	}
	return toReturn;
}

void IndexWrapper::exportIndex(string path, indexItem itemType){

	string indexPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.export");
	switch (itemType) {
	case IndexWrapper::AUTOR:
		this->author->exportToPlainText(indexPath+path,true,false); break;
	case IndexWrapper::EDITORIAL:
		this->editorial->exportToPlainText(indexPath+path,true,false); break;
	case IndexWrapper::PALABRAS:
		this->words->printDataInFile(indexPath+path); break;
	case IndexWrapper::TITULO:
		this->tittle->printDataInFile(indexPath+path); break;
	default:
		throw "Not an item handled";
	}
}

IndexWrapper* IndexWrapper::getInstanceOfIndexWrapper(){
	if(!instance)
		instance = new IndexWrapper();
	return instance;
}

IndexWrapper* IndexWrapper::instance = NULL;

IndexWrapper::IndexWrapper(){

}


void IndexWrapper::exportInvertedList(string path)
{
	this->words->exportInvertedList(path);
}
