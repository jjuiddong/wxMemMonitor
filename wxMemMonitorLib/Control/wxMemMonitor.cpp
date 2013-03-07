
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "wx/evtloop.h"

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
void memmonitor::Loop(MSG &msg)
{
	wxEventLoop * const
		evtLoop = static_cast<wxEventLoop *>(wxEventLoop::GetActive());
	if ( evtLoop && evtLoop->PreProcessMessage(&msg) )
		return;

	if (wxTheApp) 
		wxTheApp->ProcessIdle(); // 이 함수를 호출해야 wxAuiManager Docking이 작동한다.
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


