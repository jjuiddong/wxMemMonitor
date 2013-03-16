
#include "stdafx.h"
#include "Frame.h"
#include "MemoryTree.h"
#include "LogWindow.h"
#include "PropertyWindow.h"
#include "../Control/Global.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/PropertyMaker.h"


using namespace memmonitor;

enum MAINFRAME_MENU
{
	MENU_OPEN_AUTOEXP,
	MENU_EXIT,
	MENU_HELP,
};

BEGIN_EVENT_TABLE(CFrame, wxFrame)
	 EVT_MENU (MENU_OPEN_AUTOEXP, CFrame::OnMenuOpenAutoExp)
	 EVT_MENU (MENU_EXIT, CFrame::OnMenuExit)
	 EVT_MENU (MENU_HELP, CFrame::OnMenuHelp)
END_EVENT_TABLE()


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

	CreateMenuBar();

	m_pMemTree = memTree;
	m_pLogWnd = logWnd;
	m_pPropWnd = propWnd;

	if (!IsInitSuccess)
		GetLogWindow()->PrintText( GetLastError() );
	if (IsInitSuccess)
	{
		visualizer::OpenVisualizerScript( "autoexp.txt" );
		ReadConfigFile( GetConfigFileName() );
		RepositioningWindow();
	}
}

CFrame::~CFrame()
{
	WriteWindowPosition();
	// deinitialize the frame manager
	m_mgr.UnInit();
	dia::CDiaWrapper::Get()->Release();
	sharedmemory::Release();
	visualizer::Release();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::CreateMenuBar()
{
	// file Menu
	wxMenu *fileMenu = new wxMenu;
	fileMenu->Append( MENU_OPEN_AUTOEXP, "&Open AutoExp" );
	fileMenu->Append( MENU_EXIT, "&Exit" );

	// help menu
	wxMenu *helpMenu = new wxMenu;
	helpMenu->Append( MENU_HELP, "&Help" );

	// menuBar
	wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );
	menuBar->Append(fileMenu, "&File");
	menuBar->Append(helpMenu, "&Help");
	SetMenuBar(menuBar);
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
// 
//------------------------------------------------------------------------
wxWindow* CFrame::GetPane(const std::string &name)
{
	wxAuiPaneInfoArray allpanes = m_mgr.GetAllPanes();
	for (size_t i=0; i < allpanes.size(); ++i)
	{
		if (allpanes[ i].caption == name)
			return allpanes[ i].window;
	}
	return NULL;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::OnMenuOpenAutoExp(wxCommandEvent& event)
{
	::WinExec( "notepad.exe autoexp.txt", SW_SHOW);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::OnMenuExit(wxCommandEvent& event)
{
	Close(true);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CFrame::OnMenuHelp(wxCommandEvent& event)
{

}
