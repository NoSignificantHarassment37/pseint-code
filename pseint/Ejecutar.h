#ifndef EJECUTAR_H
#define EJECUTAR_H

class Programa; 

class Ejecutar {
public:
	Ejecutar(/*const */Programa &prog) : programa(prog) { }
	void Run(int LineStart, int LineEnd = -1);
private:
	/*const */Programa &programa;
};

#endif

