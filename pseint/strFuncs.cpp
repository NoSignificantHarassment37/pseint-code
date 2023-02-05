#include "strFuncs.hpp"
#include "debug.h"


string FirstWord(const string & s) {
	size_t i=0, l=s.size();
	while (i<l && EsLetra(s[i],i!=0)) i++;
	return s.substr(0,i);
}

bool LastCharIs(const std::string & s, char c) {
	if (s.empty()) return false;
	return s[s.size()-1]==c;
}

void EraseLastChar(std::string &s) {
	_expects(not s.empty());
	s.erase(s.size()-1,1);
}

void RightTrim(string &str) {
	size_t len=str.size(), last=str.size();
	while (last>0 && str[last-1]==' ') --last;
	if (last!=len) str.erase(last);
}

void LeftTrim(string &str) {
	size_t ini=0, len=str.size();
	while (ini<len && str[ini]==' ') ++ini;
	if (ini) str.erase(0,ini);
}

void Trim(string &str) {
	LeftTrim(str); RightTrim(str);
}

std::string ToUpper(std::string a) { 
	int l=a.size();
	for (int x=0;x<l;x++)
		a[x]=toupper(a[x]);
	return a; 
}

std::string ToLower(std::string a) { 
	int l=a.size();
	for (int x=0;x<l;x++)
		a[x]=tolower(a[x]);
	return a; 
}

bool RightCompare(string a, string b) { 
	if (a.size()<b.size()) return false;
	return (a.substr(a.size()-b.size())==b); /// @todo: mejorar 
}
