#include "Ejecutar.h"
#include <string>
#include "global.h"
#include "intercambio.h"
#include "utils.h"
#include <iostream>
#include "new_evaluar.h"
#include "new_memoria.h"
#include "zcurlib.h"
#include "new_programa.h"
#include "new_funciones.h"
using namespace std;

// ********************* Ejecutar un Bloque de Instrucciones **************************
// Ejecuta desde linestart+1 hasta lineend inclusive, o hasta finproceso/finsubproceso si lineend es -1.
// Las variables aux?, tmp? y tipo quedaron del código viejo, se reutilizan para diferentes
// cosas, por lo que habría que analizarlas y cambiarlas por varias otras variables con scope y 
// nombres mas claros... por ahora cambie las obvias y reduje el scope de las que quedaron, pero falta...
void Ejecutar(int LineStart, int LineEnd) {
	// variables auxiliares
	// Ejecutar el bloque
	int line=LineStart-1;
	while (true) {
		line++;
		if (LineEnd!=-1 && line>LineEnd) break; 
		
		/*const */auto &inst = programa[line];
		
		switch (inst.type) {
			case IT_FINPROCESO: {
				auto &inst_impl = getImpl<IT_FINPROCESO>(inst);
				Inter.OnAboutToEndFunction();
				_pos(line);
				if (inst_impl.principal) {
					_sub(line,"Finaliza el algoritmo");
				} else {
					_sub(line,string("Se sale del subproceso ")+getImpl<IT_FINPROCESO>(inst).nombre);
					Inter.OnFunctionOut();
				}
			} return;
			case IT_PROCESO: {
				auto &inst_impl = getImpl<IT_PROCESO>(inst);
				Inter.OnFunctionIn(inst_impl.nombre);
				_pos(line);
				_sub(line,string(inst_impl.principal?"El algoritmo comienza con el proceso ":"Se ingresa en el subproceso ")+inst_impl.nombre);
				continue;
			} break;
			case IT_BORRARPANTALLA: {
				_pos(line);
				if (for_test) cout<<"***LimpiarPantalla***"<<endl; else { clrscr(); gotoXY(1,1); }
				_sub(line,"Se borra la pantalla");
			} break;
			case IT_ESPERARTECLA: {
				_pos(line);
				_sub_msg(line,"Se espera a que el usuario presione una tecla.");
				_sub_raise();
				if (for_test) cout<<"***EsperarTecla***"<<endl; else getKey();
				_sub_wait();
			} break;
			case IT_INVOCAR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_INVOCAR>(inst);
				tipo_var tipo=vt_desconocido;
				_sub(line,string("Se va a invocar al subproceso")+inst_impl.nombre);
				EvaluarFuncion(EsFuncion(inst_impl.nombre),inst_impl.args,tipo,false);
			} break;

			// ----------- ESCRIBIR ------------ //
			case IT_ESCRIBIR: {
				_pos(line);
				auto &inst_impl = getImpl<IT_ESCRIBIR>(inst);
				// Separar parametros
				for(size_t i_expr=0;i_expr<inst_impl.expressiones.size();++i_expr) {
					const string &expression = inst_impl.expressiones[i_expr];
					if (colored_output) setForeColor(COLOR_OUTPUT);
					if (with_io_references) Inter.SendIOPositionToTerminal(i_expr+1);
					_sub(line,string("Se evalúa la expresion: ")+expression);
					DataValue res = Evaluar(expression);
					if (res.IsOk()) {
						string ans = res.GetForUser(); fixwincharset(ans);
						cout<< ans <<flush; // Si es variable, muestra el contenido
						_sub(line,string("Se muestra en pantalla el resultado: ")+res.GetForUser());
					}
				}
				if (inst_impl.saltar) cout<<endl; else cout<<flush;
			} break;
			
			// ------------- LEER --------------- //
			case IT_LEER: {
				_pos(line);
				auto &inst_impl = getImpl<IT_LEER>(inst);
				for(size_t i_var = 0; i_var<inst_impl.variables.size(); ++i_var) {
					string variable = inst_impl.variables[i_var];  // es copia por CheckDims
					
					if (lang[LS_FORCE_DEFINE_VARS] && !memoria->EstaDefinida(variable)) {
						ExeError(208,"Variable no definida ("+variable+").");
					}
					tipo_var tipo=memoria->LeerTipo(variable);
					const int *dims=memoria->LeerDims(variable);
					size_t pp=variable.find("(");
					if (dims && pp==string::npos)
						ExeError(200,"Faltan subindices para el arreglo ("+variable+").");
					else if (!dims && pp!=string::npos)
						ExeError(201,"La variable ("+variable.substr(0,pp)+") no es un arreglo.");
					if (dims) {
						_sub(line,string("Se analizan las dimensiones de ")+variable);
						CheckDims(variable);
						_sub(line,string("El resultado es ")+variable);
					}
					if (tipo.read_only)
						ExeError(322,string("No se puede modificar la variable ")+variable);
					
					if (with_io_references) Inter.SendIOPositionToTerminal(i_var+1);
					if (colored_output) setForeColor(COLOR_INFO);
					cout<<"> "<<flush;
					if (colored_output) setForeColor(COLOR_INPUT);
					// Leer dato
					_sub_msg(line,"Se espera a que el usuario ingrese un valor y presiones enter."); // tipo?
					_sub_raise();
					
					string aux1;
					if (!predef_input.empty() || noinput) {
						if (predef_input.empty()) ExeError(214,"Sin entradas disponibles.");
						aux1=predef_input.front(); predef_input.pop(); cout<<aux1<<endl;
						_sub_wait();
					} else {
						aux1=getLine();  
						if (for_eval) {
							if (aux1=="<{[END_OF_INPUT]}>") {
								cout << "<<No hay más datos de entrada>>" << endl; exit(0);
							}
							cout<<aux1<<endl; // la entrada en psEval es un stream separado de la salida, entonces la reproducimos alli para que la salida contenga todo el "dialogo"
						}
					}
					
					fixwincharset(aux1,true); // "descorrige" para que al corregir no traiga problemas
					
					string auxup=ToUpper(aux1);
					if (auxup=="VERDADERO" || auxup=="FALSO") aux1=auxup;
					
					if (tipo==vt_logica && aux1.size()==1 && (toupper(aux1[0])=='F'||aux1[0]=='0')) aux1=FALSO;
					if (tipo==vt_logica && aux1.size()==1 && (toupper(aux1[0])=='V'||aux1[0]=='1')) aux1=VERDADERO;
					tipo_var tipo2 = GuestTipo(aux1);
					if (!tipo.set(tipo2)) 
						ExeError(120,string("No coinciden los tipos (")+variable+").");
					else if (tipo==vt_numerica_entera && tipo.rounded && aux1.find(".",0)!=string::npos)
						ExeError(313,string("No coinciden los tipos (")+variable+"), el valor ingresado debe ser un entero.");
					if (Inter.subtitles_on) {
						string name = variable; 
						for (char &c:name) {
							if (c=='(') c='[';
							if (c==')') c=']';
						}
						_sub(line,string("El valor ingresado se almacena en ")+name);
					}
					memoria->DefinirTipo(variable,tipo);
					memoria->EscribirValor(variable,DataValue(tipo,aux1));
				}
			} break;
			
			// ------------- DIMENSION --------------- //
			case IT_DIMENSION: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_DIMENSION>(inst);
				for(size_t i=0;i<inst_impl.nombres.size();++i) { 
					const string &nombre = inst_impl.nombres[i];
					const string tamanios = inst_impl.tamanios[i];
					
					// Separar indices
					int anid_parent=0, cant_dims=0;
					for(size_t i=0;i<tamanios.size();i++) {
						while (i<tamanios.size() && !(anid_parent==0 && (tamanios[i]==','||tamanios[i]==')'))) {
							if (tamanios[i]=='(') anid_parent++;
							else if (tamanios[i]==')') anid_parent--;
							i++;
						}
						cant_dims++;
					}
					int *dim = new int[cant_dims+1]; dim[0]=cant_dims; // arreglo para las dimensiones
					int last=0, num_idx=0; anid_parent = 0;
					if (lang[LS_ALLOW_DINAMYC_DIMENSIONS]) { _sub(line,string("Se evalúan las expresiones para cada dimensión del arreglo ")+nombre); }
					for(size_t i=0;i<tamanios.size();i++) {
						while (i<tamanios.size() && !(anid_parent==0 && tamanios[i]==',')) {
							if (tamanios[i]=='(') anid_parent++;
							else if (tamanios[i]==')') anid_parent--;
							i++;
						}
						DataValue index = Evaluar(tamanios.substr(last,i-last));
						if (!index.CanBeReal()) ExeError(122,"No coinciden los tipos.");
						dim[++num_idx] = index.GetAsInt();
						if (dim[num_idx]<=0) {
							ExeError(274,"Las dimensiones deben ser mayores a 0.");
						}
						last=i+1;
					}
					if (Inter.subtitles_on) {
						string aux;
						for(int i=1;i<=dim[0];i++) aux+="x"+IntToStr(dim[i]);
						aux[0]=' ';
						_sub(line,string("Se crea el arreglo ")+nombre+" de"+aux+" elementos");
					}
					if (memoria->HaSidoUsada(nombre)||memoria->LeerDims(nombre))
						ExeError(123,"Identificador en uso.");
					if (dim!=0) memoria->AgregarArreglo(nombre, dim);
				}
			} break;
			
			// ------------- DEFINICION --------------- //
			case IT_DEFINIR: {
				_pos(line);
				const auto &inst_impl = getImpl<IT_DEFINIR>(inst);
				for(const string &var : inst_impl.variables) {
					if (memoria->EstaDefinida(var) || memoria->EstaInicializada(var)) 
						ExeError(124,string("La variable (")+var+") ya estaba definida.");
					memoria->DefinirTipo(var,inst_impl.tipo,inst_impl.tipo.rounded);
					if (inst_impl.tipo==vt_numerica) {
						if (inst_impl.tipo.rounded) {
							_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Numérico(Entero).");
						} else {
							_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Numérico(Real).");
						}
					} else if (inst_impl.tipo==vt_caracter) {
						_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Caracter/Cadena de Caracteres.");
					} else if (inst_impl.tipo==vt_logica) {
						_sub(line,string("Se define el tipo de la variable \"")+var+"\" como Lógico.");
					} 
				}
			} break;
			
			// ------------- ESPERAR un tiempo --------------- //
			case IT_ESPERAR: {
				_pos(line);
				auto &inst_impl = getImpl<IT_ESPERAR>(inst);
				string tiempo = inst_impl.tiempo; 
				int factor = inst_impl.factor;
				_sub(line,string("Se evalúa la cantidad de tiempo: ")+tiempo);
				DataValue time = Evaluar(tiempo);
				if (!time.CanBeReal()) ExeError(219,string("La longitud del intervalo debe ser numérica."));
				else {
					_sub(line,string("Se esperan ")+time.GetForUser()+(factor==1?" milisengudos":" segundos"));
					if (for_test) cout<<"***Esperar"<<time.GetAsInt()*factor<<"***"<<endl;
					else if (!Inter.subtitles_on) Sleep(time.GetAsInt()*factor);
				}
			} break;
			
			// ------------- ASIGNACION --------------- //
			case IT_ASIGNAR: {
				_pos(line);
				auto &inst_impl = getImpl<IT_ASIGNAR>(inst);
				string var = inst_impl.variable; // es copia por CheckDims
				const string &valor = inst_impl.valor;
				if (lang[LS_FORCE_DEFINE_VARS] && !memoria->EstaDefinida(var)) {
					ExeError(211,string("La variable (")+var+") no esta definida.");
				}
				// verificar indices si es arreglo
				if (memoria->LeerDims(var)) {
					if (var.find("(",0)==string::npos)
						ExeError(200,"Faltan subindices para el arreglo ("+var+").");
					else
						CheckDims(var);
				} else if (var.find("(",0)!=string::npos) {
					ExeError(201,"La variable ("+var.substr(0,var.find("(",0))+") no es un arreglo.");
				}
				// evaluar expresion
				_sub(line,string("Se evalúa la expresion a asignar: ")+valor);
				DataValue result = Evaluar(valor);
				// comprobar tipos
				tipo_var tipo_aux1 = memoria->LeerTipo(var);
				if (!tipo_aux1.can_be(result.type))
					ExeError(125,"No coinciden los tipos.");
				if (tipo_aux1.read_only)
					ExeError(322,string("No se puede modificar la variable ")+var);
				else if (tipo_aux1==vt_numerica_entera && tipo_aux1.rounded && result.GetAsInt()!=result.GetAsReal())
					ExeError(314,"No coinciden los tipos, el valor a asignar debe ser un entero.");
				_sub(line,string("El resultado es: ")+result.GetForUser());
				// escribir en memoria
				if (Inter.subtitles_on and valor!=var) { 
					string name = var; 
					for (char &c:name) {
						if (c=='(') c='[';
						if (c==')') c=']';
					}
					_sub(line,string("El resultado se guarda en ")+name);
				}
				result.type.rounded=false; // no forzar a entera la variable en la que se asigna
				memoria->DefinirTipo(var,result.type);
				memoria->EscribirValor(var,result);
			} break;
			
			// ---------------- SI ------------------ //
			case IT_SI: {
				auto &inst_impl = getImpl<IT_SI>(inst);
				_pos(line);
				_sub(line,string("Se evalúa la condición para Si-Entonces: ")+inst_impl.condicion);
				tipo_var tipo;
				bool condition_is_true = Evaluar(inst_impl.condicion,vt_logica).GetAsBool();
				if (tipo!=vt_error) {
					// Buscar hasta donde llega el bucle
					int anidamiento=0, line_sino=-1,line_finsi=line+1; 
					while (!(anidamiento==0 && programa[line_finsi]==IT_FINSI)) {
						// Saltear bucles anidados
						if (anidamiento==0 && programa[line_finsi]==IT_SINO) line_sino=line_finsi; 
						else if (programa[line_finsi]==IT_SI) anidamiento++;
						else if (programa[line_finsi]==IT_FINSI) anidamiento--;
						line_finsi++;
					}
					// ejecutar lo que corresponda
					if (condition_is_true) {
						_sub(line+1,"El resultado es Verdadero, se sigue por la rama del Entonces");
						if (line_sino==-1) line_sino=line_finsi;
						Ejecutar(line+2,line_sino-1); // ejecutar salida por verdadero
					} else {
						if (line_sino!=-1) {
							line = line_sino;
							_pos(line);
							_sub(line,"El resultado es Falso, se sigue por la rama del SiNo");
							Ejecutar(line+1,line_finsi-1); // ejecutar salida por falso
						} else {
							_sub(line,"El resultado es Falso, no se hace nada");
						}
					}
					// marcar la salida
					line=line_finsi;
					_pos(line);
					_sub(line,"Se sale de la estructura Si-Entonces");
				} else {
					ExeError(275,"No coinciden los tipos.");
				}
			} break;
			
			// ---------------- MIENTRAS ------------------ //
			case IT_MIENTRAS: {
				_pos(line);
				const string &condicion = getImpl<IT_MIENTRAS>(inst).condicion;
				_sub(line,string("Se evalúa la condición para Mientras: ")+condicion);
				tipo_var tipo;
				bool condition_is_true = Evaluar(condicion,vt_logica).GetAsBool();
				if (tipo!=vt_error) {
					int line_finmientras = line+1, anidamiento=0; // Buscar hasta donde llega el bucle
					while (!(anidamiento==0 && programa[line_finmientras]==IT_FINMIENTRAS)) {
						// Saltear bucles anidados
						if (programa[line_finmientras]==IT_MIENTRAS) anidamiento++;
						else if (programa[line_finmientras]==IT_FINMIENTRAS) anidamiento--;
						line_finmientras++;
					}
					while (condition_is_true) {
						_sub(line,"La condición es Verdadera, se iniciará una iteración.");
						Ejecutar(line+1,line_finmientras-1);
						_pos(line);
						_sub(line,string("Se evalúa nuevamente la condición: ")+condicion);
						condition_is_true = Evaluar(condicion,vt_logica).GetAsBool();
					}
					line=line_finmientras;
					_pos(line);
					_sub(line,"La condición es Falsa, se sale de la estructura Mientras.");
				}
			} break;
			
			// ---------------- REPETIR HASTA QUE ------------------ //
			case IT_REPETIR: {
				_pos(line);
				int line_hastaque=line+1, anidamiento=0; // Buscar hasta donde llega el bucle
				while (!(anidamiento==0 && (programa[line_hastaque]==IT_HASTAQUE))) {
					// Saltear bucles anidados
					if (programa[line_hastaque]==IT_REPETIR) anidamiento++;
					else if (programa[line_hastaque]==IT_HASTAQUE) anidamiento--;
					line_hastaque++;
				}
				// cortar condicion de cierre
				const string &condicion = getImpl<IT_HASTAQUE>(programa[line_hastaque]).condicion;
				bool valor_verdad = getImpl<IT_HASTAQUE>(programa[line_hastaque]).mientras_que;
				_sub(line,"Se ejecutarán las acciones contenidas en la estructura Repetir");
				tipo_var tipo;
				bool should_continue_iterating=true;
				while (should_continue_iterating) {
					Ejecutar(line+1,line_hastaque-1);
					// evaluar condicion y seguir
					_pos(line_hastaque);
					_sub(line_hastaque,string("Se evalúa la condición: ")+condicion);
					should_continue_iterating = Evaluar(condicion,vt_logica).GetAsBool()==valor_verdad;
					if (should_continue_iterating)
						_sub(line_hastaque,string("La condición es ")+(valor_verdad?VERDADERO:FALSO)+", se contiúa iterando.");
				} while (should_continue_iterating);
				line=line_hastaque;
				_sub(line_hastaque,string("La condición es ")+(valor_verdad?FALSO:VERDADERO)+", se sale de la estructura Repetir.");
			} break;
			
			// ------------------- PARA --------------------- //
			case IT_PARA: {
				auto &inst_impl = getImpl<IT_PARA>(inst);
				_pos(line);
				const string &contador = inst_impl.contador;
				memoria->DefinirTipo(contador,vt_numerica);
				if (lang[LS_PROTECT_FOR_COUNTER]) memoria->SetearSoloLectura(contador,true);
				
				const string &expr_ini = inst_impl.val_ini;
				_sub(line,string("Se evalúa la expresion para el valor inicial: ")+expr_ini);
				DataValue res_ini = Evaluar(expr_ini,vt_numerica);
				if (!res_ini.CanBeReal()) ExeError(126,"No coinciden los tipos."); /// @todo: parece que esto no es posible, salta antes adentro del evaluar
				
				bool positivo; // para saber si es positivo o negativo
				DataValue res_paso(vt_numerica,"1"), res_fin;
				if (inst_impl.paso.empty()) { // si no hay paso adivinar
					res_fin = Evaluar(inst_impl.val_fin,vt_numerica);
					if (lang[LS_DEDUCE_NEGATIVE_FOR_STEP] && res_ini.GetAsReal()>res_fin.GetAsReal()) {
						_sub(line,"Se determina que el paso será -1.");
						positivo=false; res_paso.SetFromInt(-1);
					} else {
						_sub(line,"Se determina que el paso será +1.");
						positivo=true; res_paso.SetFromInt(1);
					}
				} else { // si hay paso tomar ese
					const string &expr_fin = inst_impl.val_fin;
					res_fin = Evaluar(expr_fin,vt_numerica);
					const string &expr_paso = inst_impl.paso;
					_sub(line,string("Se evalúa la expresion para el paso: ")+expr_paso);
					res_paso = Evaluar(expr_paso,vt_numerica);
					positivo = res_paso.GetAsReal()>=0;
				}
				
				// Buscar hasta donde llega el bucle
				int line_finpara=line+1, anidamiento=0;
				while (!(anidamiento==0 && programa[line_finpara]==IT_FINPARA)) {
					// Saltear bucles anidados
					if (programa[line_finpara]==IT_PARA||programa[line_finpara]==IT_PARACADA) anidamiento++;
					else if (programa[line_finpara]==IT_FINPARA) anidamiento--;
					line_finpara++;
				}
				
				_sub(line,string("Se inicializar el contador ")+contador+" en "+res_ini.GetForUser());
				memoria->EscribirValor(contador,res_ini); // inicializa el contador
				string comp=positivo?"<=":">=";
				do {
					/// @todo: cuando memoria maneje DataValues usar el valor del contador directamente desde ahi en lugar de evaluar
					_sub(line,string("Se compara el contador con el valor final: ")+contador+"<="+res_fin.GetForUser());
					DataValue res_cont = Evaluar(contador,vt_numerica);
					if ( positivo ? (res_cont.GetAsReal()>res_fin.GetAsReal()) : (res_cont.GetAsReal()<res_fin.GetAsReal()) ) break;
					_sub(line,"La expresión fue Verdadera, se iniciará una iteración.");
					Ejecutar(line+1,line_finpara-1);
					_pos(line);
					res_cont = Evaluar(contador,vt_numerica); // pueden haber cambiado a para el contador!!!
					DataValue new_val = DataValue::MakeReal(res_cont.GetAsReal()+res_paso.GetAsReal());
					memoria->EscribirValor(contador,new_val);
					_sub(line,string("Se actualiza el contador, ahora ")+contador+" vale "+new_val.GetAsString()+".");
				} while(true);
				if (lang[LS_PROTECT_FOR_COUNTER]) {
					memoria->SetearSoloLectura(contador,false);
					memoria->Desinicializar(contador);
				}
				line=line_finpara;
				_pos(line);
				_sub(line,"Se sale de la estructura repetitiva Para.");
			} break;
			
			// ------------------- PARA CADA --------------------- //
			case IT_PARACADA: {
				auto &inst_impl = getImpl<IT_PARACADA>(inst);
				bool primer_iteracion=true; _pos(line);
				const string &identificador = inst_impl.identificador;
				const string &arreglo = inst_impl.arreglo;
				
				int line_finpara=line+1, anidamiento=0; // Buscar hasta donde llega el bucle
				while (!(anidamiento==0 && programa[line_finpara]==IT_FINPARA)) {
					// Saltear bucles anidados
					if (programa[line_finpara]==IT_PARA||programa[line_finpara]==IT_PARACADA) anidamiento++;
					else if (programa[line_finpara]==IT_FINPARA) anidamiento--;
					line_finpara++;
				}
				
				const int *dims=memoria->LeerDims(arreglo);
				if (!dims) ExeError(276,"La variable ("+arreglo+") no es un arreglo.");
				int nelems=1; // cantidad total de iteraciones
				for (int i=1;i<=dims[0];i++) nelems*=dims[i];
				
				// bucle posta
				_sub(line,string("El arreglo \"")+arreglo+"\" contiene "+IntToStr(nelems)+" elementos. Se comienza a iterar por ellos.");
				for (int i=0;i<nelems;i++) {
					// armar expresion del elemento (ej: A[1])
					string elemento=")";
					int naux=1;
					for (int j=dims[0];j>0;j--) {
						elemento=string(",")+IntToStr((lang[LS_BASE_ZERO_ARRAYS]?0:1)+((i/naux)%dims[j]))+elemento;
						naux*=dims[j];
					}
					elemento=arreglo+"("+elemento.substr(1);
					// asignar el elemento en la variable del bucle
					if (primer_iteracion) primer_iteracion=false; else { _pos(line); }
					_sub(line,identificador+" será equivalente a "+elemento+" en esta iteración.");
					if (!memoria->DefinirTipo(identificador,memoria->LeerTipo(elemento)))
						ExeError(277,"No coinciden los tipos.");
					memoria->EscribirValor(identificador,memoria->LeerValor(elemento));
					// ejecutar la iteracion
					Ejecutar(line+1,line_finpara-1);
					// asignar la variable del bucle en el elemento
					memoria->DefinirTipo(elemento,memoria->LeerTipo(identificador));
					memoria->EscribirValor(elemento,memoria->LeerValor(identificador));
				}
				memoria->Desinicializar(identificador);
				line=line_finpara; // linea del finpara
				_pos(line);
				_sub(line,"Se sale de la estructura repetitiva Para Cada.");
				
			} break;
			
			// ------------------- SEGUN --------------------- //
			case IT_SEGUN: {
				const string &expr_control = getImpl<IT_SEGUN>(inst).expresion; // Cortar la variable (sacar SEGUN y HACER)
				tipo_var tipo_master=vt_caracter_o_numerica;
				_pos(line);
				_sub(line,string("Se evalúa la expresion: ")+expr_control);
				DataValue val_control = Evaluar(expr_control,tipo_master); // evaluar para verificar el tipo
				if (!val_control.CanBeReal()&&(lang[LS_INTEGER_ONLY_SWITCH]||!val_control.CanBeString())) {
					if (!lang[LS_INTEGER_ONLY_SWITCH]) 
						ExeError(205,"La expresión del SEGUN debe ser de tipo numerica o caracter.");
					else
						ExeError(206,"La expresión del SEGUN debe ser numerica.");
				}
				_sub(line,string("El resultado es: ")+val_control.GetForUser());
				int line_finsegun=line+1, anidamiento=0; // Buscar hasta donde llega el bucle
				while (!(anidamiento==0 && programa[line_finsegun].type==IT_FINSEGUN)) {
					// Saltear bucles anidados
					if (programa[line_finsegun]==IT_SEGUN) anidamiento++;
					else if (programa[line_finsegun]==IT_FINSEGUN) anidamiento--;
					line_finsegun++;
				}
				int line_opcion=line; bool encontrado=false; anidamiento=0;
				while (!encontrado && ++line_opcion<line_finsegun) {
					InstructionType instruction_type=programa[line_opcion].type;
					if (instruction_type==IT_SEGUN) anidamiento++;
					else if (instruction_type==IT_FINSEGUN) anidamiento--;
					else if ((instruction_type==IT_OPCION||instruction_type==IT_DEOTROMODO) && anidamiento==0) {
						if (instruction_type==IT_DEOTROMODO) { 
							_pos(line_opcion);
							_sub(line_opcion,"Se ingresará en la opción De Otro Modo");
							encontrado=true; break;
						}
						else {
							const auto &posibles_valores = getImpl<IT_OPCION>(programa[line_opcion]).expresiones;
							for(const std::string &expr_opcion : posibles_valores) {
								_pos(line_opcion);
								_sub(line_opcion,string("Se evalúa la opcion: ")+expr_opcion);
								DataValue val_opcion = Evaluar(expr_opcion,tipo_master);
								if (!val_opcion.CanBeReal()&&(lang[LS_INTEGER_ONLY_SWITCH]||!val_opcion.CanBeString())) ExeError(127,"No coinciden los tipos.");
								// evaluar la condicion (se pone como estaban y no los resultados de la evaluaciones de antes porque sino las variables indefinida pueden no tomar el valor que corresponde
								if (Evaluar(string("(")+expr_control+")=("+expr_opcion+")").GetAsBool()) {
									_sub(line_opcion,"El resultado coincide, se ingresará en esta opción.");
									encontrado=true; break;
								} else {
									_sub(line_opcion,string("El resultado no coincide: ")+val_opcion.GetForUser());
								}
							}
						}
					}
				}
				if (encontrado) { // si coincide, buscar el final del bucle
					int line_finopcion=line_opcion+1; anidamiento=0; // Buscar hasta donde llega ese caso
					while (!(anidamiento==0 && ((programa[line_finopcion]==IT_FINSEGUN) || (programa[line_finopcion]==IT_OPCION||programa[line_finopcion]==IT_DEOTROMODO)) )) {
						// Saltear bucles anidados
						if (programa[line_finopcion]==IT_SEGUN) anidamiento++;
						else if (programa[line_finopcion]==IT_FINSEGUN) anidamiento--;
						line_finopcion++;
					}
					Ejecutar(line_opcion+1,line_finopcion-1); 
				}
				line=line_finsegun;
				_pos(line);
				_sub(line,"Se sale de la estructura Segun.	");
			} break;
			
			// ya deberíamos haber cubierto todas las opciones
			default:
				ExeError(0,"Ha ocurrido un error interno en PSeInt.");
		} // switch 
	} // while
}
