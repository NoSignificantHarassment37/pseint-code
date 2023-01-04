#ifndef EXPORT_PYTHON3_H
#define EXPORT_PYTHON3_H

#include "ExporterBase.hpp"
using namespace std;

class Python3Exporter : public ExporterBase {
	
	int version;
	
	bool import_pi;
	bool import_sleep;
	bool import_sqrt;
	bool import_log;
	bool import_exp;
	bool import_sin;
	bool import_cos;
	bool import_tan;
	bool import_asin;
	bool import_acos;
	bool import_atan;
	bool import_randint;
	bool use_subprocesos;
	
protected:
	
	void header(t_output &out);
	void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	
	void definir(t_output &prog, t_arglist &arglist, string tipo, std::string tabs) override;
	void dimension(t_output &prog, t_arglist &args, std::string tabs) override;
	
	void esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) override;
	void esperar_tecla(t_output &prog, std::string tabs) override;
	void borrar_pantalla(t_output &prog, std::string tabs) override;
	void invocar(t_output &prog, string param, std::string tabs) override;
	void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	void leer(t_output &prog, t_arglist args, std::string tabs) override;
	void asignacion(t_output &prog, string param1, string param2, std::string tabs) override override;
	void si(t_output &prog, t_proceso_it r, t_proceso_it q, t_proceso_it s, std::string tabs) override;
	void mientras(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
	void repetir(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void para(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void paracada(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void comentar(t_output &prog, string text, std::string tabs) override;
	
public:
	string make_string (string cont);
	string function(string name, string args);
	string get_constante(string name);
	string get_operator(string op, bool for_string=false);	
	void translate(t_output &out, t_programa &prog);
	Python3Exporter(int version);
	
};

#endif

