#ifndef SYNCHECK_H
#define SYNCHECK_H
#include <string>
#include "instruccion.h"
using namespace std;

class Programa;

class SynCheck {
public:
	SynCheck(Programa &p) : programa(p) { }
	int Run();
private:
	Programa &programa;
	int Instrucciones();
	void InformUnclosedLoops(std::vector<int> &bucles, int &errores);
	static void Operadores(const int &x, string &cadena, int &errores, InstructionType instruction_type);
	static std::pair<string,bool> Normalizar(string &cadena);
	static void Condiciones(string &cadena, int &errores);
	friend int ParseInspection(string &exp);
};

int ParseInspection(string &exp);
bool SirveParaReferencia(const string &s);

#endif

