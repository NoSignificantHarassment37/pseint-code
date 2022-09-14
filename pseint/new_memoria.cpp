#include "new_memoria.h"
#include "Funciones.hpp"

Memoria *memoria;
bool Memoria::EsArgumento (const string &nom) const {
	if (!funcion) return false;
	for(int i=1;i<=funcion->cant_arg;i++) 
		if (funcion->nombres[i]==nom) return true;
	return false;
}

