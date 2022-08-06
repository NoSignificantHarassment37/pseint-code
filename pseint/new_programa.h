#ifndef NEW_PROGRAMA_H
#define NEW_PROGRAMA_H
#include <string>
#include <vector>
#include "instruccion.h"
#include "intercambio.h"

class Programa {
	vector<Instruccion> v;
	int cant_lines;
	int ref_point; // marcador para apuntar a una linea, y que PushBack y Erase lo actualicen si corresponde
public:
	Programa():cant_lines(0),ref_point(0) {}
	Instruccion &operator[](int i) { 
		return v[i];
	}
	void Insert(int pos,const string &inst, int num_line=-1, int num_inst=-1) { 
		if (ref_point>=pos) ref_point++;
		cant_lines++;
		if (pos) {
			if (num_line==-1) num_line=v[pos-1].num_linea;
			if (num_inst==-1) num_inst=v[pos-1].num_instruccion+1;
		}
		v.insert(v.begin()+pos,Instruccion(inst,num_line,num_inst));
	}
	void PushBack(string inst) { 
		if (ref_point>=int(v.size())) ref_point++; 
		v.push_back(Instruccion(inst,++cant_lines,1));
	}
	void Erase(int i) { 
		if (ref_point>=i) ref_point--; 
		v.erase(v.begin()+i); cant_lines--;
	}
	int GetSize() { 
		return cant_lines;
	}
	int GetInstSize() { 
		return v.size();
	}
//	Instruccion GetLoc(int x, string s) { 
//		return Instruccion(s,v[x].num_linea,v[x].num_instruccion);
//	}
	Instruccion GetLoc(int x, InstructionType t) { 
		return Instruccion(t,v[x].num_linea,v[x].num_instruccion);
	}
	void HardReset() { 
		v.clear(); cant_lines=0;
	}
	void SetRefPoint(int x=-1) { 
		ref_point=x;
	}
	int GetRefPoint() { 
		return ref_point;
	}
	static string aux_type2str(const string &inst, const string &args) {
		return args.empty()?inst:(args==";"?(inst+args):(inst+" "+args));
	}
};

extern Programa programa;

#endif

