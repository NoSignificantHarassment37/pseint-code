#ifndef CASEMAP_H
#define CASEMAP_H
#include <map>
#include <string>
using namespace std;


/// @todo: convertir en clase... y ver si es una exageracion recibir el runtime solo para los nombres de subs

class RunTime;

// guarda en los nombres originales de las variables (antes de pasar a mayusculas)
// para usar en la salida (para diagramas de flujo mayormente), 
// NULL quiere decir que no se hace
extern map<string,string> *case_map; 

void CaseMapFill(const RunTime &rt, string &s);
void CaseMapPurge();
void CaseMapApply(const RunTime &rt, string &s, bool and_fix_parentesis);
void InitCaseMap();

#endif

