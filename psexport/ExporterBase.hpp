#ifndef EXPORTER_BASE_HPP
#define EXPORTER_BASE_HPP
#include <string>
#include <map>
#include <stack>
#include <vector>
#include "defines.h"
#include "../pseint/FuncsManager.hpp"
#include "../pseint/Evaluar.hpp"
#include "../pseint/new_memoria.h"
#include "../pseint/utils.h"
#include "../pseint/RunTime.hpp"
#include "exportexp.h"

class ExporterBase {

	static RunTime m_runtime; // static para que se comparta por ej entre el de tipos y el de otro lenguaje cuando se encadenan
	
protected:
	
	void set_memoria(string key);
	
	void replace_var(t_output &out, string src, string dst);

	// funciones de utilería para las demás
	vector<string> aux_varnames;
	virtual string get_aux_varname(string pref);
	virtual void release_aux_varname(string vname);
	void init_header(t_output &out, string comment_pre, string comment_post="");
	void bloque(t_output &prog, t_proceso_it r, t_proceso_it q, std::string tabs);
	string get_arg(string args, int cual);
	string make_dims(const int *tdims, string c1="[", string c2=",", string c3="]", bool numbers=true);
	void crop_name_and_dims(string decl, string &name, string &dims, string par_open="[", string comma=",", string par_close="]");
	void fix_dims(string &dims, string par_open="[", string comma=",", string par_close="]");
	bool es_cadena_constante(const string &s);
	bool es_numerica_constante(const string &s);
	bool es_numerica_entera_constante(const string &s);
	bool replace_all(string &str, string from, string to);
	void load_subs_in_funcs_manager(Programa &prog);
	std::string expresion(string exp) { return ::expresion(m_runtime,exp); }
	std::string expresion(string exp, tipo_var &t) { return ::expresion(m_runtime,exp,t); }
	
	// funciones que traducen instrucciones y estructuras de control
	virtual void esperar_tiempo(t_output &prog, string tiempo, bool mili, std::string tabs)=0;
	virtual void esperar_tecla(t_output &prog, std::string tabs)=0;
	virtual void borrar_pantalla(t_output &prog, std::string tabs)=0;
	virtual void invocar(t_output &prog, string func_name, string args, std::string tabs)=0;
	virtual void escribir(t_output &prog, t_arglist expresiones, bool saltar, std::string tabs)=0;
	virtual void leer(t_output &prog, t_arglist variables, std::string tabs)=0;
	virtual void asignacion(t_output &prog, string variable, string valor, std::string tabs)=0;
	virtual void si(t_output &prog, t_proceso_it ir_si, t_proceso_it it_fin, t_proceso_it s, std::string tabs)=0;
	virtual void mientras(t_output &prog, t_proceso_it it_mientras, t_proceso_it it_fin, std::string tabs)=0;
	virtual void segun(t_output &prog, std::vector<t_proceso_it> &its_opciones, std::string tabs)=0;
	virtual void repetir(t_output &prog, t_proceso_it it_rep, t_proceso_it it_hasta, std::string tabs)=0;
	virtual void paracada(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs)=0;
	virtual void para(t_output &prog, t_proceso_it it_para, t_proceso_it it_fin, std::string tabs)=0;
	virtual void dimension(t_output &prog, t_arglist &nombres, t_arglist &tamanios, std::string tabs);
	virtual void definir(t_output &prog, t_arglist &variables, tipo_var tipo, std::string tabs);
	virtual void comentar(t_output &prog, string text, std::string tabs);
	
public:
	
	RunTime &GetRT() { return m_runtime; }
	
	// funciones para traducir expresiones
	
	/**
	* @brief retorna un identificador de variable, convirtiendo todo a minuscula y agregando algun prefijo si es necesario (como el $ en php)
	*
	* @param nombre de la variable, puede ser en mayusculas o minusculas, pero el valor de retorno debe ser siempre case-insensitive
	**/
	virtual string make_varname(string varname);
	
	/**
	* @brief returns a literal string with the given contend, adding colons and escape chars if required
	*
	* @param the string content, without colons
	**/
	virtual string make_string(string cont)=0;
	
	/**
	* @brief traduce una constante
	*
	* @param name    nombre de la constante
	**/
	virtual string get_constante(string name)=0;
	
	/**
	* @brief traduce un operador
	*
	* @param name        nombre de la constante
	* @param for_string  indica si el operador se aplica sobre operandos de tipo string
	**/
	virtual string get_operator(string op, bool for_string=false)=0;
	
	/**
	* @brief traduce la llamada a una función predefinida
	*
	* @param name    nombre de la función en pseudocódigo
	* @param args    string con la lista de argumentos (incluye los paréntesis)
	**/
	virtual string function(string name, string args)=0;
	
	/**
	* @brief modifica una expresion para que sea pasada por referencia a una
	*        funcion (por ej, en C aplica el &)
	**/
	virtual std::string referencia(const std::string &exp) { return exp; }
	
	
	// función principal, que arma el esqueleto e invoca a las demas
	
	/**
	* @brief traduce el algoritmo
	*
	* @param out     argumento de salida, donde colocar las instrucciones traducidas
	* @param prog    argumento de entrada, con el algoritmo ya parseado
	**/
	virtual void translate(t_output &out, Programa &prog)=0;
	
	virtual void translate_all_procs(t_output &out_main,t_output &out_progs, Programa &prog, std::string tabs="");
	virtual void translate_all_procs(t_output &out, Programa &prog, std::string tabs="");
	virtual void translate_single_proc(t_output &out, Funcion *f, t_proceso &proc);
	
};

extern ExporterBase *exporter;

#endif

