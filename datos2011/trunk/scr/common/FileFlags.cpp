#include "FileFlags.h"
#include "../common/Resource.h"

FileFlags::FileFlags(string filepath)
{
	string flagPath = Utility::getApplicationPath() + Resource::getConfigProperties()->get("booquerio.directorio.archivos.flags");

	this->fileName = flagPath + filepath + ".flags";
	fileFlag.open(this->fileName.c_str(), ios::in | ios::out | ios::binary);
	if(fileFlag.is_open())
		loadListOfIDs();
}

FileFlags::~FileFlags() {
	//Aca guardo los registros de la lista en el archivo.
	fileFlag.close();
	remove(this->fileName.c_str());
	fopen(this->fileName.c_str(),"w+");

	saveFile();
	emptyList();
	fileFlag.close();
}

bool FileFlags::addID(int id)
{
	bool found = false;
	int actualID;
	ListIterator<int> it = listOfID.getIterator();
	while(it.hasNext() && !found){
			actualID = it.next();
			if(id == actualID)
			{
				found = true;
				break;
			}
	}
	if(!found)
	{
		listOfID.insertData(id);
	}
	return !found;
}

bool FileFlags::removeID(int ID)
{
	bool found = false;
	int actualID;
	ListIterator<int> it = listOfID.getIterator();
	while(it.hasNext() && !found){
			actualID = it.next();
			if(ID == actualID)
			{
				found = true;
			}
	}
	if(found)
	{
		listOfID.removeData(ID);
	}
	return !found;
}

void FileFlags::saveFile()
{
	ListIterator<int> it = listOfID.getIterator();
	fileFlag.open(this->fileName.c_str(), ios::in | ios::out | ios::binary);
	int count = listOfID.getSize();
	fileFlag.write((char*)&count,NUMBER_OF_IDS);
	while(it.hasNext())
	{
		int id = it.next();
		fileFlag.write((char*)&id,ID_NUMBER);
	}
}

void FileFlags::loadListOfIDs()
{
	fileFlag.seekg(0);
	char countOfIDs[NUMBER_OF_IDS];
	fileFlag.read(countOfIDs,NUMBER_OF_IDS);
	ByteString BScountOfIDs;
	BScountOfIDs.insertLast((void*)(countOfIDs),NUMBER_OF_IDS);
	int size = BScountOfIDs.readAsInt(0);

	for(int i =0;i<size;i++)
	{
		char id[ID_NUMBER];
		fileFlag.read(id,ID_NUMBER);
		ByteString stringOfByte;
		stringOfByte.insertLast((void*)id,ID_NUMBER);

		int integerID = stringOfByte.readAsInt(0);

		listOfID.insertData(integerID);
	}
}

void FileFlags::emptyList(){
	int id;
	while(!listOfID.isEmpty()){
		id = listOfID.getFirst();
		listOfID.removeData(id);
	}
}

List<int>* FileFlags::getListID()
{
	List<int>* list = new List<int>;
	ListIterator<int> it = listOfID.getIterator();

	while(it.hasNext())
	{
		int ID = it.next();
		list->insertData(ID);
	}
	emptyList();
	return list;
}

