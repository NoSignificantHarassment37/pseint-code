#include "global.h"
#include "common.h"
#include "Evaluar.hpp"
#include "intercambio.h"
#include "new_memoria.h"
#include "utils.h"
#include "zcurlib.h"
#include "FuncsManager.hpp"
#include "RunTime.hpp"

void show_user_info(string msg) {
	if (fix_win_charset) fixwincharset(msg);
	if (colored_output) setForeColor(COLOR_INFO);
	if (with_io_references) Inter.SendErrorPositionToTerminal(); // para que no asocie el error/mensaje con la última entrada/salida
	cout<<msg<<endl;
}

void show_user_info(string msg1, int num, string msg2) {
	if (fix_win_charset) { fixwincharset(msg1); fixwincharset(msg2); }
	if (colored_output) setForeColor(COLOR_INFO);
	if (with_io_references) Inter.SendErrorPositionToTerminal(); // para que no asocie el error/mensaje con la última entrada/salida
	cout<<msg1<<num<<msg2<<endl;
}


// ***************** Control de Errores y Depuración **********************

// ------------------------------------------------------------
//    Informa un error en tiempo de ejecucion
// ------------------------------------------------------------
void ExeError_impl(int num, string s) { 
	if (Inter.EvaluatingForDebug()) {
		Inter.SetError(string("<<")+s+">>");
	} else {
		if (raw_errors) {
			cout<<"=== Line "<<Inter.GetLocation().linea<<": ExeError "<<num<<endl;
			exit(0);
		}
		if (colored_output) setForeColor(COLOR_ERROR);
		if (with_io_references) Inter.SendErrorPositionToTerminal();
		cout<<"Lin "<<Inter.GetLocation().linea<<" (inst "<<Inter.GetLocation().linea<<"): ERROR "<<num<<": "<<s<<endl;
		for(int i=Inter.GetBacktraceLevel()-1;i>0;i--) {  
			FrameInfo fi=Inter.GetFrame(i);
			cout<<"...dentro del subproceso "<<fi.func_name<<", invocado desde la línea "<<fi.loc.linea<<"."<<endl;
		}
		if (ExeInfoOn) {
			ExeInfo<<"Lin "<<Inter.GetLocation().linea<<" (inst "<<Inter.GetLocation().instruccion<<"): ERROR "<<num<<": "<<s<<endl;
			for(int i=Inter.GetBacktraceLevel()-1;i>0;i--) {  
				FrameInfo fi=Inter.GetFrame(i);
				ExeInfo<<"Lin "<<fi.loc.linea<<" (inst "<<fi.loc.instruccion<<"): ";
				ExeInfo<<"...dentro del subproceso "<<fi.func_name<<", invocado desde aquí."<<endl;
			}
			ExeInfo<<"*** Ejecucion Interrumpida. ***"<<endl;
		} 
		if (wait_key) {
			show_user_info("*** Ejecución Interrumpida. ***");
		}
//		Inter.AddError(s,Inter.GetLocation().linea);
		if (ExeInfoOn) ExeInfo.close();
		if (wait_key) getKey();
		exit(0);
	}
}

// ------------------------------------------------------------
//    Informa un error de syntaxis antes de la ejecucion
// ------------------------------------------------------------
void SynError_impl(int num,string s) { 
	SynError_impl(num,s,Inter.GetLocation());
}

void SynError_impl(int num,string s, CodeLocation loc) { 
#ifdef _FOR_PSEXPORT
	return;
#endif
	if (raw_errors) {
		cout<<"=== Line "<<loc.linea<<": SynError "<<num<<endl;
		return;
	}
	if (Inter.EvaluatingForDebug()) {
		Inter.SetError(string("<<")+s+">>");
	} else {
		if (colored_output) setForeColor(COLOR_ERROR);
		if (with_io_references) Inter.SendErrorPositionToTerminal(); // para que no asocie el error con la última entrada/salida
		cout<<"Lin "<<loc.linea;
		if (loc.instruccion>0) cout<<" (inst "<<loc.instruccion<<")";
		cout<<": ERROR "<<num<<": "<<s<<endl;
		if (ExeInfoOn) {
			ExeInfo<<"Lin "<<loc.linea;
			if (loc.instruccion>0) ExeInfo<<" (inst "<<loc.instruccion<<")";
			ExeInfo<<": ERROR "<<num<<": "<<s<<endl;
		}
	}
}


// ------------------------------------------------------------
//    Comprueba que sea un identificador correcto y unico
//    A diferencia del anterior, no tiene en cuenta las
//  funciones predefinidas.
// ------------------------------------------------------------
bool CheckVariable(RunTime &rt, string str, int errcode) { 
	size_t pi=str.find("(",0);
	if (pi!=string::npos && str[str.size()-1]==')') {
		CheckDims(rt,str);
		str.erase(pi,str.size()-pi); // si es arreglo corta los subindices
	}
	bool ret=true;
	if (!EsLetra(str[0]))
		ret=false;
	for (int x=0;x<(int)str.size();x++) {
		if (!EsLetra(str[x]) && (str[x]<'0' || str[x]>'9') && str[x]!='_')
			ret=false;
	}
	// Comprobar que no sea palabra reservada
	if (rt.funcs.IsFunction(str)) 
		ret=false;
	else if (str=="LEER" || str=="ESCRIBIR" || str=="MIENTRAS" || str=="HACER" || str=="SEGUN" || str=="VERDADERO" || str=="FALSO" || str=="PARA")
		ret=false;
	else if (str=="REPETIR" || str=="SI" || str=="SINO" || str=="ENTONCES" || str=="DIMENSION" || str=="PROCESO" || str=="FINSI" ||  str=="" || str=="FINPARA")
		ret=false;
	else if (str=="FINSEGUN" || str=="FINPROCESO" || str=="FINMIENTRAS" || str=="HASTA" || str=="DEFINIR" || str=="COMO")
		ret=false;
	else if (lang[LS_ENABLE_USER_FUNCTIONS] && (str=="FINSUBPROCESO" || str=="SUBPROCESO" ||str=="FINFUNCION" || str=="FUNCION" ||str=="FINFUNCIÓN" || str=="FUNCIÓN") )
		ret=false;
	if (!ret && errcode!=-1) rt.err.SyntaxError(errcode,string("Identificador no válido (")+str+")."); 
	return ret;
}

// ----------------------------------------------------------------------
//    Compara los comienzos de dos cadenas (case sensitive)
// ----------------------------------------------------------------------
bool LeftCompare(string a, string b) {  
	// compara los caracteres de la izquierda de a con b
	bool ret;
	if (a.size()<b.size()) ret=false; else {
		a.erase(b.size(),a.size()-b.size());
		if (a==b) ret=true; else ret=false; }
	return ret; }

// ----------------------------------------------------------------------
//    Compara los comienzos de dos cadenas (case insensitve)
// ----------------------------------------------------------------------
//bool LeftCompareNC(string a, string b){  
//	// compara los caracteres de la izquierda de a con b
//	for (int x=0;x<(int)a.size();x++)
//		if (a[x]>96 && a[x]<123) a[x]-=32;
//	for (int x=0;x<(int)b.size();x++)
//		if (b[x]>96 && b[x]<123) b[x]-=32;
//	bool ret;
//	if (a.size()<b.size()) ret=false; else {
//		a.erase(b.size(),a.size()-b.size());
//		if (a==b) ret=true; else ret=false; }
//	return ret; 
//}

// ----------------------------------------------------------------------
//    Compara las terminaciones de dos cadenas (case sensitve)
// ----------------------------------------------------------------------
bool RightCompare(string a, string b) { 
	// compara los caracteres de la derecha de a con b
	bool ret;
	if (a.size()<b.size()) ret=false; else {
		a.erase(0,a.size()-b.size());
		if (a==b) ret=true; else ret=false; }
	return ret; 
}

// ----------------------------------------------------------------------
//    Compara las terminaciones de dos cadenas (case insensitve)
// ----------------------------------------------------------------------
//bool RightCompareNC(string a, string b) { 
//	// compara los caracteres de la derecha de a con b
//	for (int x=0;x<(int)a.size();x++)
//		if (a[x]>96 && a[x]<123) a[x]-=32;
//	for (int x=0;x<(int)b.size();x++)
//		if (b[x]>96 && b[x]<123) b[x]-=32;
//	bool ret;
//	if (a.size()<b.size()) ret=false; else {
//		a.erase(0,a.size()-b.size());
//		if (a==b) ret=true; else ret=false; }
//	return ret; 
//}

// ----------------------------------------------------------------------
//    Compara parte de una cadena con otra (case insensitve)
// ----------------------------------------------------------------------
bool MidCompareNC(string a, string b, int from) { 
	unsigned int to=from+a.size();
	// controla los tamaños y corta la parte de interes
	if (b.size()<to) return false;
	b.erase(to,b.size());
	b.erase(0,from);
	for (int x=0;x<(int)a.size();x++)
		if (a[x]>96 && a[x]<123) a[x]-=32;
	for (int x=0;x<(int)b.size();x++)
		if (b[x]>96 && b[x]<123) b[x]-=32;
	bool ret;
	// compara los caracteres de la derecha de a con b
	if (a.size()<b.size()) ret=false; else {
		a.erase(0,a.size()-b.size());
		if (a==b) ret=true; else ret=false; }
	return ret; 
}

// ----------------------------------------------------------------------
//    Pasa una cadena a mayusculas
// ----------------------------------------------------------------------
string ToUpper(string a) { 
	int l=a.size();
	for (int x=0;x<l;x++)
		a[x]=toupper(a[x]);
	return a; 
}

// ----------------------------------------------------------------------
//    Pasa una cadena a mayusculas
// ----------------------------------------------------------------------
string ToLower(string a) { 
	int l=a.size();
	for (int x=0;x<l;x++)
		a[x]=tolower(a[x]);
	return a; 
}

// ----------------------------------------------------------------------
//    Averigua el tipo de variable para un dato
// ----------------------------------------------------------------------
tipo_var GuestTipo(string str) { 
	tipo_var ret=vt_desconocido;
	string strb;
	strb=str;
	if (str.size()==0) ret=vt_desconocido; else {
		if (strb=="VERDADERO" || strb=="FALSO") {
			ret=vt_caracter_o_logica;
		} else {
			ret=vt_caracter_o_numerica;
			bool punto=false;
			for (int x=0;x<(int)str.size();x++) {
				if (str[x]=='.') {
					if (punto) { ret=vt_caracter; break; }
					else punto=true;
				} else if (str[x]<'0' || str[x]>'9') {
					if (x||(str[x]!='-'&&str[x]!='+')) {
						ret=vt_caracter;
						break;
					}
				}
			}
		}
	}
	return ret;
}

// ----------------------------------------------------------------------
//    Reemplaza una cadena por otra si es que se encuentra
// ----------------------------------------------------------------------
bool ReplaceIfFound(string &str, string str1, string str2, bool saltear_literales) { 
	size_t x=str.find(str1,0);
	size_t ox=0;
	bool ret=false;
	while (x!=string::npos) {
		if (saltear_literales) {
			size_t xc=str.find("\'",ox);
			if (xc!=string::npos && xc<x) {
				xc=str.find("\'",xc+1);
				if (xc==string::npos) break;
				x=str.find(str1,ox=(xc+1));
				continue;
			}
		}
		ret=true;
		str.erase(x,str1.size());
		str.insert(x,str2);
		x=str.find(str1,ox=(x+str2.size()));
	}
	return ret; 
}


// **************************************************************************

// funciones auxiliares para psexport
inline int max(int a,int b){ return (a>b)?a:b; }
inline int min(int a,int b){ return (a<b)?a:b; }
string CutString(string s, int a, int b){
	string r=s;
	r.erase(0,a);
	if (b!=0) r.erase(r.size()-b,b);
	return r;
}

// determina si una letra puede ser parte de una palabra clave o identificador
bool parteDePalabra(char c) {
	return (EsLetra(c) || c=='_' || (c>='0' && c<='9'));
}

void fixwincharset(string &s, bool reverse) {
	if (!fix_win_charset) return;
	if (reverse) {
		for(unsigned int i=0;i<s.size();i++) { 
			char &c=s[i];
			if (c==-96) c='á';
			else if (c==-126) c='é';
			else if (c==-95) c='í';
			else if (c==-94) c='ó';
			else if (c==-93) c='ú';
			else if (c==-75) c='Á';
			else if (c==-112) c='É';
			else if (c==-42) c='Í';
			else if (c==-32) c='Ó';
			else if (c==-23) c='Ú';
			else if (c==-92) c='ñ';
			else if (c==-91) c='Ñ';
			else if (c==-83) c='¡';
			else if (c==-88) c='¿';
			else if (c==-127) c='ü';
			else if (c==-102) c='Ü';
		}
	} else {
		for(unsigned int i=0;i<s.size();i++) { 
			char &c=s[i];
			if (c=='á') c=-96;
			else if (c=='é') c=-126;
			else if (c=='í') c=-95;
			else if (c=='ó') c=-94;
			else if (c=='ú') c=-93;
			else if (c=='Á') c=-75;
			else if (c=='É') c=-112;
			else if (c=='Í') c=-42;
			else if (c=='Ó') c=-32;
			else if (c=='Ú') c=-23;
			else if (c=='ñ') c=-92;
			else if (c=='Ñ') c=-91;
			else if (c=='¡') c=-83;
			else if (c=='¿') c=-88;
			else if (c=='ü') c=-127;
			else if (c=='Ü') c=-102;
		}
	}
}

std::string NextToken(const std::string &cadena, int &p) {
	int l=cadena.size();
	while (p<l && (cadena[p]==' ' || cadena[p]=='\t')) p++;
	if (p==l) return "";
	int p1=p;
	if (EsLetra(cadena[p])) {
		while (p<l && parteDePalabra(cadena[p]) ) p++;
	} else if ((cadena[p]>='0'&&cadena[p]<='9')||cadena[p]=='.') {
		while (p<l && ((cadena[p]>='0'&&cadena[p]<='9')||cadena[p]=='.')) p++;
	} else if (cadena[p]=='\"'||cadena[p]=='\'') {
		while (p<l && cadena[p]!='\"'&&cadena[p]!='\'' ) p++;
	} else if (cadena[p]==')') {
		p++; return ")";
	} else if (cadena[p]=='(') {
		p++; return "(";
	} else { // operador? todavia no se usa este caso
		if (p<l) p++;
		while (p<l && !parteDePalabra(cadena[p]) && cadena[p]!=' ' && cadena[p]!='\t' && cadena[p]!=')' && cadena[p]!='(' && cadena[p]!=',' && cadena[p]!=';' && cadena[p]!='\"' && cadena[p]!='\'') p++;
	}
	return cadena.substr(p1,p-p1);
}

// arma el objeto Funcion analizando la cabecera (cadena, que viene sin la palbra PROCESO/SUBPROCESO)
// pero no lo registra en el mapa de subprocesos

std::unique_ptr<Funcion> MakeFuncionForSubproceso(RunTime &rt, const std::string &cadena, bool es_proceso) {
	return MakeFuncionForSubproceso(rt,SepararCabeceraDeSubProceso(cadena),es_proceso);
}

std::unique_ptr<Funcion> MakeFuncionForSubproceso(RunTime &rt, const FuncStrings &parts, bool es_proceso) {
	ErrorHandler &err_handler = rt.err;
	
	auto the_func = std::make_unique<Funcion>(0); 
	
	// parsear nombre y valor de retorno
	if (parts.ret_id.empty()) {
		the_func->tipos[0]=vt_error; // para que cuando la quieran usar en una expresión salte un error, porque evaluar no verifica si se devuelve algo porque se use desde Ejecutar parala instrucción INVOCAR
	} else {
		if (es_proceso) err_handler.SyntaxError(242,"El proceso principal no puede retornar ningun valor.");
		the_func->nombres[0] = parts.ret_id;
		CheckVariable(rt,the_func->nombres[0]);
	}
	if (parts.nombre.empty()) { 
		err_handler.SyntaxError(40,es_proceso?"Falta nombre de proceso.":"Falta nombre de subproceso."); 
		static int untitled_functions_count=0; // para numerar las funciones sin nombre
		the_func->id = string("<sin_nombre>")+IntToStr(++untitled_functions_count);
	} else {
		the_func->id = parts.nombre;
		if (rt.funcs.IsFunction(parts.nombre))
			err_handler.SyntaxError(243,string("Ya existe otro proceso/subproceso con el mismo nombre(")+parts.nombre+").");
		else if (not CheckVariable(rt,parts.nombre))
			err_handler.SyntaxError(244,string("El nombre del proceso/subproceso(")+parts.nombre+") no es válido.");
	}
	// argumentos
	if (not parts.args.empty())	{
		int p = 0; std::string tok = NextToken(parts.args,p);
		if (tok=="(") {// si habia argumentos
			if (es_proceso) err_handler.SyntaxError(246,"El proceso principal no puede recibir argumentos.");
			bool closed = false; 
			std::string tok = NextToken(parts.args,p);
			while (tok!="") {
				if (tok==")") { closed=true; break; }
				else if (tok==",") { 
					err_handler.SyntaxError(247,"Falta nombre de argumento.");
					tok=NextToken(parts.args,p);
				} else {
					CheckVariable(rt,tok);
					the_func->AddArg(tok);
					tok=NextToken(parts.args,p);
					if (tok=="POR") {
						tok=NextToken(parts.args,p);
						if (tok!="REFERENCIA"&&tok!="COPIA"&&tok!="VALOR")
							err_handler.SyntaxError(248,"Tipo de pasaje inválido, se esperaba Referencia, Copia o Valor.");
						else the_func->SetLastPasaje(tok=="REFERENCIA"?PP_REFERENCIA:PP_VALOR);
						tok=NextToken(parts.args,p);
					} else if (tok!="," && tok!=")" && tok!="")
						err_handler.SyntaxError(249,"Se esperaba coma(,) o parentesis ()).");
					if (tok==",") { tok=NextToken(parts.args,p); }
				}
			}
			if (not closed) err_handler.SyntaxError(250,"Falta cerrar lista de argumentos.");
			else if (NextToken(parts.args,p).size()) err_handler.SyntaxError(251,"Se esperaba fin de linea.");
		} else {
			// si no habia argumentos no tiene que haber nada
			if (es_proceso) err_handler.SyntaxError(252,"Se esperaba el fin de linea."); 
			else {
				if (the_func->nombres[0].size()) err_handler.SyntaxError(253,"Se esperaba la lista de argumentos, o el fin de linea.");
				else err_handler.SyntaxError(254,"Se esperaba la lista de argumentos, el signo de asignación, o el fin de linea.");
			}
		}
	}
	return the_func;
}

FuncStrings SepararCabeceraDeSubProceso(string cadena) {
	FuncStrings ret;
	int p_args = 0; 
	ret.nombre = NextToken(cadena,p_args);
	int p_aux = p_args;
	std::string tok = NextToken(cadena,p_aux);
	if (tok=="="||tok=="<-") { 
		ret.ret_id = std::move(ret.nombre); 
		ret.nombre = NextToken(cadena,p_aux);
		p_args = p_aux;
	}
	ret.args = cadena.substr(p_args);
	if (ret.nombre.empty()) {
		static int untitled_functions_count=0;
		ret.nombre = string("<sin_nombre>")+IntToStr(++untitled_functions_count);
	}
	return ret;
}

string FirstWord(const string & s) {
	size_t i=0, l=s.size();
	while (i<l && EsLetra(s[i],i!=0)) i++;
	return s.substr(0,i);
}

