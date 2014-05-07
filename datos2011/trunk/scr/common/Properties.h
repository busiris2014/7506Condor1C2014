#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#define PROP_SEPARADOR "="
#define PROP_COMENTARIO "#"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <map>
#include "../Utility/Utility.h"

using namespace std;

class Properties {
	private:
		string fileName;
		ifstream* file;
		map<string,string>* properties;

		void initialize();
		void processLine(string line);

	public:

		Properties();
		Properties(string fileName);
		virtual ~Properties(void);

		string get(string key);
		int getAsInt(string key);
		char getAsChar(string key);

		void setFileName(string fileName);
};

#endif
