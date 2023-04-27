#include <iostream>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include "../wxPSeInt/osdep.h"
#include "Application.h"
#include "mxFrame.h"
#include "version.h"
#include <wx/filename.h>

IMPLEMENT_APP(mxApplication)
	
static wxString EscapeString(wxString what) {
	bool have_space=false;
	for(unsigned int i=0;i<what.Len();i++)
		if (what[i]=='\"'
#ifdef __WIN32__
			||(what[i]=='\\'&&what[i+1]=='\"')
#else
			||what[i]=='\''||what[i]=='\\'
#endif
			) { what=what.Mid(0,i)+"\\"+what.Mid(i); i++; }
		else if (what[i]==' ') have_space=true;
	if (have_space) return wxString("\"")+what+"\"";
	else return what;
}

wxString DIR_PLUS_FILE(wxString dir, wxString fil) {
	if (dir.Len()==0 || (fil.Len()>1 && (fil[0]=='\\' || fil[0]=='/' || fil[1]==':')))
		return fil;
	else if (dir.Last()==wxFileName::GetPathSeparator())
		return dir+fil;
	else
		return dir+wxFileName::GetPathSeparator()+fil;
}

bool mxApplication::OnInit() {
	
	_handle_version_query("psTerm",false);
	
	OSDep::AppInit();
	
	srand(time(0));
	
	
	wxFileName f_path = wxGetCwd(); 
	f_path.MakeAbsolute();
	wxString cmd_path = f_path.GetFullPath();
	wxFileName f_cmd(argv[0]);
	wxFileName f_zpath = f_cmd.GetPathWithSep();
	f_zpath.MakeAbsolute();
	wxString zpath(f_zpath.GetPathWithSep());
	bool flag=false;
	if (f_zpath!=f_path) {
		if ( (flag=(wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("pseint.dir"))) || wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("PSeInt.dir")))) ) )
			wxSetWorkingDirectory(zpath);
#ifdef __APPLE__
		else if ( (flag=(wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../Resources/pseint.dir"))) ||wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../Resources/PSeInt.dir")))) ) ) {
			zpath = DIR_PLUS_FILE(zpath,_T("../Resources"));
			wxSetWorkingDirectory(zpath);
		}
#elif !defined(__WIN32__)
		else if ( (flag=(wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../pseint.dir"))) ||wxFileName::FileExists(DIR_PLUS_FILE(zpath,_T("../PSeInt.dir")))) ) ) {
			zpath = DIR_PLUS_FILE(zpath,_T(".."));
			wxSetWorkingDirectory(zpath);
		}
#endif
		else 
				 zpath = cmd_path;
	}
	
	bool no_arg=false, debug=false;
	long port=-1, src_id=-1;
	win_props props;
	wxString command;
	for(int i=1;i<argc;i++) {
		if (no_arg) {
			if (command.Len()) command<<" ";
			command<<EscapeString(argv[i]);
		} else {
			wxString arg(argv[i]);
			if (arg.StartsWith("--")) {
				if (arg=="--") {
					no_arg=true;
				} else if (arg=="--debugmode") {
					debug=true;
				} else if (arg=="--darktheme") {
					props.dark_theme=true;
				} else if (arg=="--alwaysontop") {
					props.always_on_top=true;
				} else if (arg.StartsWith("--id=")) {
					if (!arg.AfterFirst('=').ToLong(&src_id)) return false;
				} else if (arg.StartsWith("--left=")) {
					if (!arg.AfterFirst('=').ToLong(&props.left)) return false;
					props.set_left=true;
				} else if (arg.StartsWith("--right=")) {
					if (!arg.AfterFirst('=').ToLong(&props.right)) return false;
					props.set_right=true;
				} else if (arg.StartsWith("--top=")) {
					if (!arg.AfterFirst('=').ToLong(&props.top)) return false;
					props.set_top=true;
				} else if (arg.StartsWith("--bottom=")) {
					if (!arg.AfterFirst('=').ToLong(&props.bottom)) return false;
					props.set_bottom=true;
				} else if (arg.StartsWith("--width=")) {
					if (!arg.AfterFirst('=').ToLong(&props.width)) return false;
				} else if (arg.StartsWith("--height=")) {
					if (!arg.AfterFirst('=').ToLong(&props.height)) return false;
				} else if (arg.StartsWith("--port=")) {
					if (!arg.AfterFirst('=').ToLong(&port)) return false;
				} else if (arg.StartsWith("--font=")) {
					wxString fontspec = arg.AfterFirst('=');
					props.font_name = fontspec.BeforeLast(':');
					if (!fontspec.AfterLast(':').ToLong(&props.font_size)) return false;
				}
			} else {
				if (!no_arg) return false;
				command=EscapeString(argv[i]);
//				no_arg=true;
			}
		}
	}
	wxImage::AddHandler(new wxPNGHandler);
//	wxFont::AddPrivateFont("Inconsolata-Regular.ttf"); // already done win win_props
	new mxFrame(command,port,src_id,debug,props);
	return true;
}
