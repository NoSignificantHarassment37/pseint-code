#ifndef EXPORT_EXP_H
#define EXPORT_EXP_H

#include<iostream>
#include<iomanip>
#include<stack>
#include "new_memoria.h"
#include "RunTime.hpp"
#include "strFuncs.hpp"

class ExpIntrucciones;

extern bool input_base_zero_arrays;
extern bool output_base_zero_arrays;

// conversion de expresiones
string modificarConstante(string s,int diff);

string buscarOperando(const string &exp, int comienzo, int direccion);

string colocarParentesis(const string &exp);

string sumarOrestarUno(string exp, bool sumar);

string expresion(RunTime &rt, string exp, tipo_var &tipo);

string expresion(RunTime &rt, string exp);

string invert_expresion(std::string expr);

#endif
