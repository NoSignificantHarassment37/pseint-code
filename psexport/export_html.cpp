#if 0
#include "export_html.h"
#include "../pseint/FuncsManager.hpp"
#include "../pseint/utils.h"

HTMLExporter::HTMLExporter ( ) :JavaScriptExporter(true) {
	
}

void HTMLExporter::translate (t_output & out, t_programa & prog) {
	insertar(out,"<!DOCTYPE html>");
	insertar(out,"<HTML>");
	insertar(out,"\t<HEAD>");
	insertar(out,string("\t\t<TITLE>")+GetRT().funcs.GetMainName()+"</TITLE>");
	insertar(out,"\t</HEAD>");
	insertar(out,string("\t<BODY onload=\"")+ToLower(GetRT().funcs.GetMainName())+"();\">");
	insertar(out,"\t\t<SCRIPT type=\"text/javascript\">");
	JavaScriptExporter::translate(out,prog);
	insertar(out,"\t\t</SCRIPT>");
	insertar(out,"\t</BODY>");
	insertar(out,"</HTML>");
}

#endif
