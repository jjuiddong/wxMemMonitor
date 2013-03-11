
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "wx/evtloop.h"
#include "Global.h"
#include "../dia/DiaWrapper.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace memmonitor
{
	// functions
	bool	LoadConfigFile(const std::string &fileName);

}

using namespace memmonitor;


//------------------------------------------------------------------------
// load config file
// init dia library
// init shared memory
//------------------------------------------------------------------------
bool memmonitor::Init(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName)
{
	SetExecuteType(type);

	if (INNER_PROCESS == type)
	{
		if (!LoadConfigFile( configFileName ))
		{
			dia::CDiaWrapper::Get()->Release();
			sharedmemory::Release();
			return false;
		}

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
	dia::CDiaWrapper::Get()->Release();
	sharedmemory::Release();

	if (INNER_PROCESS == GetExecuteType())
	{
		if ( wxTheApp )
			wxTheApp->OnExit();
		wxEntryCleanup();
	}
}


//------------------------------------------------------------------------
// open configfile, json file format 
//------------------------------------------------------------------------
bool	memmonitor::LoadConfigFile(const std::string &fileName)
{
	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName.c_str(), props);
		string pdbPath = props.get<string>("pdbpath");
		string shareMemoryName = props.get<string>("sharedmemoryname");

		// Pdb Load
		if (!dia::CDiaWrapper::Get()->Init(pdbPath))
		{
			SetErrorMsg(
				common::format("%s Pdb 파일이 없습니다.", pdbPath.c_str()) );
			return false;
		}
		if (!sharedmemory::Init(shareMemoryName, sharedmemory::SHARED_CLIENT))
		{
			SetErrorMsg(
				common::format("%s  이름의 공유메모리가 없습니다.", 
				shareMemoryName.c_str()) );
			return false;
		}
	}
	catch (std::exception &e)
	{
		SetErrorMsg(
			common::format( "\"%s\" json script Err!! [%s]",  
			fileName.c_str(), e.what()) );
		return false;
	}

	return true;
}


//------------------------------------------------------------------------
// return last error message
//------------------------------------------------------------------------
const std::string& memmonitor::GetLastError()
{
	return GetErrorMsg();
}

