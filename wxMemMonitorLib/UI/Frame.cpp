
#include "stdafx.h"
#include "Frame.h"
#include "MemoryTree.h"
#include "LogWindow.h"
#include "PropertyWindow.h"
#include "../Control/Global.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/PropertyMaker.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace memmonitor;


CFrame::CFrame(wxWindow* parent) : wxFrame(parent, -1, _("wxMemMonitor"),
	wxDefaultPosition, wxSize(400,500),
	wxDEFAULT_FRAME_STYLE)
{
	const bool IsInitSuccess = InitMemoryMonitor( GetConfigFileName() );

	// notify wxAUI which frame to use
	m_mgr.SetManagedWindow(this);

	CMemoryTree* memTree = new CMemoryTree(this);
	CLogWindow *logWnd = new CLogWindow(this);
	CPropertyWindow *propWnd = new CPropertyWindow(this);

	// add the panes to the manager
	m_mgr.AddPane(logWnd, wxBOTTOM, wxT("Log Window"));
	m_mgr.AddPane(memTree, wxLEFT, wxT("Memory Tree"));
	m_mgr.AddPane(propWnd, wxCENTER, wxT("Properties"));

	// tell the manager to "commit" all the changes just made
	m_mgr.Update();

	m_pMemTree = memTree;
	m_pLogWnd = logWnd;
	m_pPropWnd = propWnd;

	if (!IsInitSuccess)
		GetLogWindow()->PrintText( GetLastError() );
	if (IsInitSuccess)
	{
		visualizer::OpenVisualizerScript( "autoexp.txt" );
		LoadConfigFile( GetConfigFileName() );
	}
}

CFrame::~CFrame()
{
	// deinitialize the frame manager
	m_mgr.UnInit();
	dia::CDiaWrapper::Get()->Release();
	sharedmemory::Release();
	visualizer::Release();
}


//------------------------------------------------------------------------
// add PropertyWindow
//------------------------------------------------------------------------
bool CFrame::AddPropertyWindow(const wxString &symbolName )
{
	CPropertyWindow *propWnd = new CPropertyWindow(this);
	m_mgr.AddPane(propWnd, wxRIGHT, symbolName);
	m_mgr.Update();

	propWnd->UpdateSymbol(symbolName);

	return true;
}


//------------------------------------------------------------------------
// open configfile, json file format 
//------------------------------------------------------------------------
bool	CFrame::InitMemoryMonitor(const std::string &configFileName)
{
	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(configFileName.c_str(), props);
		string pdbPath = props.get<string>("pdbpath");
		string shareMemoryName = props.get<string>("sharedmemoryname");

		// Pdb Load
		if (!dia::CDiaWrapper::Get()->Init(pdbPath))
		{
			SetErrorMsg(
				common::format("%s Pdb 파일이 없습니다.\n", pdbPath.c_str()) );
			return false;
		}
		if (!sharedmemory::Init(shareMemoryName, sharedmemory::SHARED_CLIENT))
		{
			SetErrorMsg(
				common::format("%s  이름의 공유메모리가 없습니다.\n", 
				shareMemoryName.c_str()) );
			return false;
		}
	}
	catch (std::exception &e)
	{
		SetErrorMsg(
			common::format( "\"%s\" json script Err!! [%s]\n",  
			configFileName.c_str(), e.what()) );
		return false;
	}

	return true;
}


//------------------------------------------------------------------------
// read configfile, read attribute "property" : [ ] 
// then create property window
//------------------------------------------------------------------------
bool CFrame::LoadConfigFile(const std::string &fileName)
{
	if (fileName.empty())
	{
		GetLogWindow()->PrintText( "not found config file\n" );
		return false;
	}

	try
	{
		// boost property tree
		using boost::property_tree::ptree;
		using std::string;
		ptree props;
		boost::property_tree::read_json(fileName.c_str(), props);
		ptree &childs = props.get_child("property");
		BOOST_FOREACH(ptree::value_type &vt, childs)
		{
			const string name = vt.second.get<string>("symbolname");
			// 			int val[ 4] = {0,};
			// 			int cnt =0;
			// 			ptree::assoc_iterator it = vt.second.find("rect");
			// 			if (vt.second.not_found() != it)
			// 			{
			// 				ptree &rect = vt.second.get_child("rect");
			// 				BOOST_FOREACH(ptree::value_type &vt, rect)
			// 				{
			// 					val[ cnt++] = atoi(vt.second.data().c_str());
			// 				}
			// 			}
			//			CRect propertyRect(val[0], val[1], val[2], val[3]);
			//CString symbolName = common::str2wstr(name).c_str();
			AddPropertyWindow( name );
		}
	}
	catch (std::exception &e)
	{
		GetLogWindow()->PrintText( 
			wxString("(") + fileName.c_str() + ") " + e.what() );
	}

	return true;
}
