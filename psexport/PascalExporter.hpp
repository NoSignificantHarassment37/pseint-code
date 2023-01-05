#ifndef PASCAL_EXPORTER_HPP
#define PASCAL_EXPORTER_HPP

#include "ExporterBase.hpp"
using namespace std;

class PascalExporter : public ExporterBase {
	
protected:
	
	bool uses_crt;
	bool uses_sin_tipo;
	bool uses_math;
	bool uses_sysutils;
	bool uses_randomize;
	bool has_matrix_func;
	bool use_arreglo_max;
	
	string get_tipo(map<string,tipo_var>::iterator &mit, bool for_func=false, bool by_ref=false); // se usa tanto desde el otro get_tipo como desde declarar_variables
	void declarar_variables(t_output &prog);
	string get_tipo(string name, bool by_ref=false, bool do_erase=true); // solo se usa para cabeceras de funciones
	void header(t_output &out);
	void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	
	void esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) override;
	void esperar_tecla(t_output &prog, std::string tabs) override;
	void borrar_pantalla(t_output &prog, std::string tabs) override;
	void invocar(t_output &prog, std::string func_name, std::string args, std::string tabs) override;
	void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	void leer(t_output &prog, t_arglist args, std::string tabs) override;
	void asignacion(t_output &prog, string param1, string param2, std::string tabs) override;
	void si(t_output &prog, t_proceso_it it_si, t_proceso_it it_sino, t_proceso_it it_fin, std::string tabs) override;
	void mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs) override;
	void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
	void repetir(t_output &prog, t_proceso_it it_repetir, t_proceso_it it_hasta, std::string tabs) override;
	void para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) override;
	void paracada(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs) override;
	void comentar(t_output &prog, string text, std::string tabs) override;
	
public:
	string make_string(string cont) override;
	string function(string name, string args) override;
	string get_constante(string name) override;
	string get_operator(string op, bool for_string=false) override;
	void translate(t_output &out, Programa &prog) override;
	PascalExporter();
	
};

#endif

