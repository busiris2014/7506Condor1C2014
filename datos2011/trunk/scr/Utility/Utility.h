#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include <string.h>
#include <sstream>
#include <list>
#include <set>
#include <map>

using namespace std;

class Utility {
public:

	static string aplicationPath;

	Utility();
	virtual ~Utility();

	static string toString(int);
	static string intToString(int i);
	static int stringToInt(string str);

	static bool empty(string str);
	static string trim(string str);
	static bool equals(string str1, string str2);
	static string concat(string str1, string str2);
	static string toLower(string str);
	static list<string> split(string texto, string separator);
	static set<string> splitToSet(string text, string separator);

	// Para los caracteres con acento, devuelve su equivalente sin acento y para las Ñ mayuscula, el caracter n
	// En otro caso devuelve vacio
	static char getNormalizedChar(char character);
	static void setApplicationPath(char* path);
	static string getApplicationPath();

	static string cleanString(string str);
	static string normalizeString(string str);

	static string dateFormat(string date);
	static int compareDate(string dateEnd,string dateBegining);
	static char* getDate();

};



#endif /* UTILITY_H_ */


