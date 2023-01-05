#ifndef QBASIC_EXPORTER_HPP
#define QBASIC_EXPORTER_HPP
#include "ExporterBase.hpp"
using namespace std;

class QBasicExporter : public ExporterBase {
	
	bool use_asin, use_acos, use_rand, use_pi;
	
	string get_tipo(map<string,tipo_var>::iterator &mit, bool for_func=false, bool by_ref=false); // se usa tanto desde el otro get_tipo como desde declarar_variables
	void declarar_variables(t_output &prog);
	string get_tipo(string name,bool by_ref=false); // solo se usa para cabeceras de funciones
	void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc) override;
	
	void invocar(t_output &prog, std::string func_name, std::string args, std::string tabs) override;
	void esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs) override;
	void esperar_tecla(t_output &prog, std::string tabs) override;
	void borrar_pantalla(t_output &prog, std::string tabs) override;
	void escribir(t_output &prog, t_arglist args, bool saltar, std::string tabs) override;
	void leer(t_output &prog, t_arglist args, std::string tabs) override;
	void asignacion(t_output &prog, string param1, string param2, std::string tabs) override;
	void si(t_output &prog, t_proceso_it r, t_proceso_it q, t_proceso_it s, std::string tabs) override;
	void mientras(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void segun(t_output &prog, std::vector<t_proceso_it> &its, std::string tabs) override;
	void repetir(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void para(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void paracada(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs) override;
	void dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs) override;
	void comentar(t_output &prog, string text, std::string tabs) override;
	
public:
	string make_string(string cont) override;
	string function(string name, string args) override;
	string get_constante(string name) override;
	string get_operator(string op, bool for_string=false) override;
	void translate(t_output &out, Programa &prog) override;
	QBasicExporter();
	
};

#endif

