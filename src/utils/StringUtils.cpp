#include "StringUtils.h"
#include <sstream>
#include <stdio.h>

using namespace std;

namespace utils
{

vector<string> split(string &str, char delim)
{
	vector<string> elems;
	stringstream ss(str);
	string item;
	
	while(getline(ss, item, delim))
		elems.push_back(item);
		
	return elems;
}

string chop(string str, string search, bool reverse)
{
	size_t pos = str.find(search);
	
	if (pos != string::npos)
	{
		if (reverse)
			str = str.substr(pos+1);
		else
			str = str.substr(0,pos);
	}
		
	return str;
}

void trim(string &str)
{
	// remove spaces
	string::size_type pos = str.find_last_not_of(' ');
	
	if(pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
	
	// and tabs
	pos = str.find_last_not_of('\t');
	
	if(pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of('\t');
		
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
	
	// and newlines
	pos = str.find_last_not_of('\n');
	
	if(pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of('\n');
		
		if(pos != string::npos) str.erase(0, pos);
	}
	else str.erase(str.begin(), str.end());
}

}
