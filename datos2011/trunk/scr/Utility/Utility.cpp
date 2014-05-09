#include "Utility.h"

string Utility::aplicationPath = "";

Utility::Utility(){
}

Utility::~Utility() {
}

string Utility::toString(int i)
{
	string num;
	int temp;
	while (i / 10 != 0)
	{
		temp = i % 10;
	    i = i / 10;
	    temp = temp + 48;
	    num = (char)temp + num;
	}
	i = i + 48;
	num = (char)i + num ;
	return num;
}

string Utility::intToString(int i)
{
	bool negative = false;
	string num;

	if (i<0)
	{
		negative = true;
		i = -1 * i;
	}

	//conversion de entero positivo
	int temp;
	while (i / 10 != 0)
	{
		temp = i % 10;
	    i = i / 10;
	    temp = temp + 48;
	    num = (char)temp + num;
	}
	i = i + 48;
	num = (char)i + num ;

	if (negative == true)
		num = "-" + num;

	return num;
}

int Utility::stringToInt(string str)
{
	int integer;
	stringstream buffer(str);
	buffer >> integer;
	return integer;
}

bool Utility::empty(string str)
{
	bool isEmpty = true;

	if (str.size() > 0)
	{
		unsigned int index = str.find_first_not_of(" \n\t");

		if (index != string::npos)
			isEmpty = false;
	}

	return isEmpty;
}

string Utility::trim(string str)
{
	if (!Utility::empty(str))
	{
		unsigned int begin = str.find_first_not_of(" \t\n");
		unsigned int end = str.find_last_not_of(" \t\n");

		string result = str.substr(begin, end+1);
		result.erase((end+1)-begin);

		return result;

	}
	else
		return "";
}

bool Utility::equals(string str1, string str2)
{
	bool areEquals = false;

	if (str1.size() == str2.size())
		areEquals = equal(str1.begin(), str1.end(), str2.begin());

	return areEquals;
}

string Utility::concat(string str1, string str2)
{
	int length = strlen(str1.c_str()) + strlen(str2.c_str());
	char* result = new char[length + 1];

	strcpy(result, str1.c_str());
	strcat(result, str2.c_str());

	return result;
}

string Utility::toLower(string str)
{
	for (unsigned int i=0; i < strlen(str.c_str()); i++)
	{
		if ((str[i]>=0x41)&&(str[i]<=0x5A))
			str[i] = str[i] + 0x20;
	}

	return str;
}

list<string> Utility::split(string text, string separator)
{
	list<string> result;
	unsigned int begin = 0;
	unsigned int end = text.find_first_of(separator, begin);
	bool finish = false;

	while (!finish)
	{
		if (end == string::npos)
		{
			end = text.size();
			finish = true;
		}

		string fragment = text.substr(begin, end);
		fragment.erase(end - begin);
		fragment = trim(fragment);

		if (!empty(fragment))
			result.push_back(fragment);

		if (!finish)
		{
			begin = end + separator.size();
			end = text.find_first_of(separator, begin);
		}
	}

	return result;
}

set<string> Utility::splitToSet(string text, string separator)
{
	set<string> result;
	unsigned int initial = 0;
	unsigned int final = text.find_first_of(separator, initial);
	bool end = false;

	while (!end)
	{
		if (final == string::npos)
		{
			final = text.size();
			end = true;
		}

		string fragment = text.substr(initial, final);
		fragment.erase(final - initial);
		fragment = Utility::trim(fragment);

		if (!Utility::empty((fragment)))
			result.insert(fragment);

		if (!end)
		{
			initial = final + separator.size();
			final = text.find_first_of(separator, initial);
		}
	}

	return result;
}

char Utility::getNormalizedChar(char character)
{
	map<char,char> extendedChars;
	map<char,char>::iterator it;
	char newChar = 0x00;

	// Vocales Minusculas con Acentos
	extendedChars['\241'] = 'a';
	extendedChars['\251'] = 'e';
	extendedChars['\255'] = 'i';
	extendedChars['\263'] = 'o';
	extendedChars['\272'] = 'u';

	// Vocales Mayusculas con Acentos
	extendedChars['\201'] = 'a';
	extendedChars['\211'] = 'e';
	extendedChars['\215'] = 'i';
	extendedChars['\223'] = 'o';
	extendedChars['\232'] = 'u';

	// Vocales Minusculas con Dieresis
	extendedChars['\244'] = 'a';
	extendedChars['\253'] = 'e';
	extendedChars['\257'] = 'i';
	extendedChars['\266'] = 'o';
	extendedChars['\274'] = 'u';

	// Vocales Mayusculas con Dieresis
	extendedChars['\204'] = 'a';
	extendedChars['\213'] = 'e';
	extendedChars['\217'] = 'i';
	extendedChars['\226'] = 'o';
	extendedChars['\234'] = 'u';

	// Vocales Minusculas con Acento Circunflejo
	extendedChars['\242'] = 'a';
	extendedChars['\252'] = 'e';
	extendedChars['\256'] = 'i';
	extendedChars['\264'] = 'o';
	extendedChars['\273'] = 'u';

	// Vocales Mayusculas con Acento Circunflejo
	extendedChars['\202'] = 'a';
	extendedChars['\212'] = 'e';
	extendedChars['\216'] = 'i';
	extendedChars['\224'] = 'o';
	extendedChars['\233'] = 'u';

	// Vocales Minusculas con Acento Grave
	extendedChars['\240'] = 'a';
	extendedChars['\250'] = 'e';
	extendedChars['\254'] = 'i';
	extendedChars['\262'] = 'o';
	extendedChars['\271'] = 'u';

	// Vocales Mayusculas con Acento Grave
	extendedChars['\200'] = 'a';
	extendedChars['\210'] = 'e';
	extendedChars['\214'] = 'i';
	extendedChars['\222'] = 'o';
	extendedChars['\231'] = 'u';

	// ñ
	extendedChars['\261'] = 'n';
	// Ñ
	extendedChars['\221'] = 'n';
	// ç
	extendedChars['\247'] = 'c';
	// Ç
	extendedChars['\207'] = 'c';
	// ã
	extendedChars['\243'] = 'a';
	// þ
	extendedChars['\276'] = ' ';

	if(extendedChars.count(character)>0)
	{
		newChar = extendedChars.find(character)->second;
	}

	return newChar;
}

void Utility::setApplicationPath(char* path)
{
	string stringPath(path);
	int index = stringPath.find_last_of('/');
	Utility::aplicationPath = stringPath.substr(0, index + 1);
}

string Utility::getApplicationPath()
{
	return Utility::aplicationPath;
}

string Utility::normalizeString(string str)
{
	string newStr = "";
	char newChar;
	unsigned int index = 0;
	int inicial = 0;

	// Se reemplazan los caracteres extendidos
	while (index < str.length())
	{
		if (str[index] == '\303')
		{
			newChar = getNormalizedChar(str[index + 1]);

			newStr = Utility::concat(newStr, str.substr(inicial, index - inicial));
			newStr = newStr + newChar;

			index += 1;
			inicial = index + 1;
		}

		index += 1;
	}

	newStr = Utility::concat(newStr, str.substr(inicial, str.length()));

	newStr = Utility::toLower(newStr);

	newStr = Utility::trim(newStr);

	return newStr;
}


string Utility::dateFormat(string date){
	struct tm tm;
	char buffer[7];
	const char formatosalida[7] = "%Y%j";

	//formato de salida:
	//"2013360" (año y dia del año)

	//formatos de entrada
	//rss
	//Thu, 08 May 2014 14:54:00 -0300
	//Thu,  8 May 2014 11:39:00 -0400
	//Thu, 8 May 2014 09:56:43 +0200
	//Thu, 08 May 2014 10:52:00 PDT
	//Tue, 06 May 2014 17:28:41 GMT
	//01 Sep 2012 00:00:01 +0200
	//twitter
	//Wed Apr 02 09:00:06 +0000 2014

	if(date.length()==31)
		{
		if(date[5] == '0')
			strptime(date.c_str(), "%a, %d %b %Y %T %z", &tm);
		else
			strptime(date.c_str(), "%a, %e %b %Y %T %z", &tm);
		}

	else if(date.length()==30)
		{
		if(date[3] == ',')
			strptime(date.c_str(), "%a,%e %b %Y %T %z", &tm);
		else
			strptime(date.c_str(), "%a %b %d %T %z %Y", &tm);
		}

	else if(date.length()==29)
		strptime(date.c_str(), "%a, %d %b %Y %T %Z", &tm);

	else if(date.length()==26)
		strptime(date.c_str(), "%d %b %Y %T %Z", &tm);

	else return 0;

	strftime (buffer,7,formatosalida,&tm);
    return buffer;
}


int Utility::compareDate(string dateEnd,string dateBegining)
{
	struct tm tmEnd,tmBegin;
	double seconds;

	strptime(dateEnd.c_str(), "%d %b %Y %T %z", &tmEnd);
	strptime(dateBegining.c_str(), "%d %b %Y %T %z", &tmBegin);

	seconds = difftime(mktime(&tmEnd),mktime(&tmBegin));

	if(seconds < 0.0)return -1;
	else if(seconds > 0.0)return 1;
	else return 0;
}




char* Utility::getDate()
{
	time_t theTime;
	time(&theTime);   // get the calendar time
	tm *t = localtime(&theTime);  // convert to local
	return asctime(t);
}

