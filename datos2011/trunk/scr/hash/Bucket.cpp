#include "Bucket.h"
#include "../common/Constantes.h"
#include "../common/ComparerRecord.h"

Bucket::Bucket(int capacity):RecordList(new ComparerRecord())
{
	this->capacity = capacity;
	this->dispersionNumber = 1;
	this->availableSpace = capacity - 1*sizeof(int);
}

Bucket::Bucket(int capacity, int dispersionNumber)
{
	this->capacity = capacity;
	this->dispersionNumber = dispersionNumber;
	this->availableSpace = capacity - 1*sizeof(int);
}

Bucket::~Bucket() {
	this->toEmpty();
}


//Formato del Bucket en cadena de bytes:
//NumDispersion|Record|....|Record|Espacio_Vacio
ByteString Bucket::Serialize(){
	ByteString byteString;
	//byteString.insertLast(&availableSpace, sizeof(int));
	byteString.insertLast(&dispersionNumber, sizeof(int));

	if(!RecordList.isEmpty())
	{
		ListIterator<Record*> it= RecordList.getIterator();
		Record* reg = NULL;

		while(it.hasNext())
		{
			reg = it.next();
			byteString.insertLast(reg->Serialize());
		}
	}

	//Completa con ceros
	byteString.fillWithEmpty(this->capacity);
	return byteString;
}

bool Bucket::Hidratate(ByteString &bytesString){
	bool success = true;
	if(int(bytesString.getSize()) != capacity)
	{
		success = false;
	}
	else
	{
		//si tenia registros los borro
		this->toEmpty();

		//Obtiene la Metadata
		dispersionNumber = bytesString.readAsInt(0);

		//Obtiene los registros
		Record* record = NULL;
		ByteString recordBytesString;
		int position = 1*sizeof(int);
		int recordSize = bytesString.readAsInt(1*sizeof(int));
		while(recordSize > 0)
		{
			record = new Record();
			recordBytesString = bytesString.read(position,recordSize);
			record->Hidratate(recordBytesString);
			success = this->insertRecord(record);

			position = position + recordSize;

			//Valida que quede el minimo recordSize que puede tener un registro
			if(position <= capacity - Record::getMinSize())
				recordSize = bytesString.readAsInt(position);
			else
				recordSize = -1;
		}
	}
	return success;
}


Record* Bucket::find(Key* key){
	Record* record = NULL;
	bool found = false;
	ListIterator<Record*> it= RecordList.getIterator();
	while(it.hasNext() && !found){
			record = it.next();
			if(*(record->getKey()) == *key) found = true;
	}
	if(!found) record = NULL;

	return record;
}

bool Bucket::insertRecord(Record* data){
	bool success = true;
	int dataSize = data->getSize();
	if(dataSize > this->availableSpace)
	{
		success = false;
	}
	else
	{
		RecordList.insertData(data);
		availableSpace = availableSpace - data->getSize();
	}

	return success;
}

bool Bucket::deleteRecord (Key* key){
	bool eliminated = false;
	Record* record = this->find(key);
	if(record){
		RecordList.removeData(record);
		availableSpace = availableSpace + record->getSize();
		delete record;
	}

	return eliminated;
}

void Bucket::toEmpty()
{
	Record* record = NULL;
	while(!RecordList.isEmpty()){
		record = RecordList.getFirst();
		RecordList.removeData(record);
		availableSpace = availableSpace + record->getSize();
		delete record;
	}
}

string Bucket::toString(bool isRecordDataInt)
{
	string byteString("(td:");
	byteString += Utility::intToString(dispersionNumber) + ")";
	byteString += "\n";
	ListIterator<Record*> it= RecordList.getIterator();
	while(it.hasNext()){
		Record* record = it.next();
		byteString += "\t\t";
		byteString += record->getKey()->toString();
		byteString += " | ";

		if (isRecordDataInt == true)
		{
			byteString += Utility::intToString(record->getData()->readAsInt(sizeof(int)*1));
			byteString += " | ";
			byteString += Utility::intToString(record->getData()->readAsInt(sizeof(int)*0));
		}
		else
			byteString += record->getData()->toString();

		byteString += "\n";
	}
	return byteString;
}

bool Bucket::isEmpty()
{
	return this->RecordList.isEmpty();
}

List<Record*>* Bucket::getRecordList()
{
	return &(this->RecordList);
}

int Bucket::getCapacity(){
	return this->capacity;
}

int Bucket::getAvailableSpace() const{
	return this->availableSpace;
}

int Bucket::getDispersionNumber() const{
	return this->dispersionNumber;
}

void Bucket::setDispersionNumber(int n){
	this->dispersionNumber = n;
}

void Bucket::setAvailableSpace(int availableSpace){
	this->availableSpace = availableSpace;
}
