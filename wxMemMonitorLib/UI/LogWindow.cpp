
#include "stdafx.h"
#include "LogWindow.h"

using namespace  memmonitor;

CLogWindow::CLogWindow(wxWindow *parent) :
	wxTextCtrl(parent, -1, wxT("LogWindow\n"), wxDefaultPosition, wxSize(200,150),
		wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER)
{

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CLogWindow::PrintText( const wxString &msg )
{
	AppendText( msg );
}
