
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "wx/evtloop.h"
#include "Global.h"
#include "../ui/LogWindow.h"
#include "../dia/DiaWrapper.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using namespace memmonitor;

//------------------------------------------------------------------------
// load config file
// init dia library
// init shared memory
//------------------------------------------------------------------------
bool memmonitor::Init(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName)
{
	SetExecuteType(type);
	SetConfigFileName(configFileName);

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

	return (GetLastError().empty()? true : false);
}


//------------------------------------------------------------------------
// must call this message loop function
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
	if (INNER_PROCESS == GetExecuteType())
	{
		if ( wxTheApp )
			wxTheApp->OnExit();
		wxEntryCleanup();
	}
}


//------------------------------------------------------------------------
// return last error message
//------------------------------------------------------------------------
const std::string& memmonitor::GetLastError()
{
	return GetErrorMsg();
}

