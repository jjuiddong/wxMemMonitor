
#include "stdafx.h"
#include "../wxMemMonitor.h"

namespace memmonitor
{
	EXECUTE_TYPE n_Type;
}

using namespace memmonitor;


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool memmonitor::Init(EXECUTE_TYPE type, HINSTANCE hInst)
{
	n_Type = type;

	if (INNER_PROCESS == type)
	{
		WXDLLIMPEXP_BASE void wxSetInstance(HINSTANCE hInst);
		wxSetInstance(hInst);
		wxApp::m_nCmdShow = SW_SHOW;
		int argc = 0;
		wxChar **argv = NULL;
		wxEntryStart(argc, argv);
		if ( !wxTheApp || !wxTheApp->CallOnInit() )
			return false;
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void memmonitor::Cleanup()
{
	if (INNER_PROCESS == n_Type)
	{
		if ( wxTheApp )
			wxTheApp->OnExit();
		wxEntryCleanup();
	}
}
