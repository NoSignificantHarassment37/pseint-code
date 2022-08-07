#ifndef CODE_H
#define CODE_H

struct CodeLocation {
	int linea, instruccion;
	CodeLocation(int _linea, int _inst):linea(_linea),instruccion(_inst) {}
};

#endif
