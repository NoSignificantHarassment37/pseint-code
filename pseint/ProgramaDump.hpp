#ifndef PROGRAMADUMP_HPP
#define PROGRAMADUMP_HPP
#include "Programa.hpp"

#ifndef _FOR_PSEXPORT
class RunTime;
void CaseMapApply(const RunTime &rt, Programa &programa, bool for_export);
#endif

Programa LoadPrograma(const std::string &fname);

bool SavePrograma(const std::string &fname, /*const*/ Programa &p);

#endif

