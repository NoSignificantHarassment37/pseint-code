#ifndef NEW_PROGRAMA_H
#define NEW_PROGRAMA_H
#include <string>
#include <vector>
#include "instruccion.h"
#include "intercambio.h"

class Programa {
	vector<Instruccion> v;
	int cant_lines;
public:
	Programa():cant_lines(0) {}
	Instruccion &operator[](int i) { 
		return v[i];
	}
	const Instruccion &operator[](int i) const { 
		return v[i];
	}
	void Insert(int pos,const string &inst, CodeLocation loc) { 
		cant_lines++;
		v.insert(v.begin()+pos,Instruccion(inst,loc));
	}
	void Insert(int pos,const string &inst) {
		Insert(pos,inst,{pos?v[pos-1].loc.linea:-1,pos?v[pos-1].loc.instruccion+1:-1});
	}
	void PushBack(string inst) { 
		v.push_back(Instruccion(inst,{++cant_lines,1}));
	}
	void Erase(int i) { 
//		if (ref_point>=i) ref_point--; 
		v.erase(v.begin()+i); cant_lines--;
	}
	int GetLinesCount() const { 
		return cant_lines;
	}
	int GetInstCount() const { 
		return v.size();
	}
	void HardReset() { 
		v.clear(); cant_lines=0;
	}
};

#endif

