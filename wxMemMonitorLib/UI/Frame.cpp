
#include "stdafx.h"
#include "Frame.h"
#include "MemoryTree.h"
#include "LogWindow.h"
#include "PropertyWindow.h"
#include "../dia/DiaWrapper.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace memmonitor;


CFrame::CFrame(wxWindow* parent) : wxFrame(parent, -1, _("wxAUI Test"),
	wxDefaultPosition, wxSize(800,600),
	wxDEFAULT_FRAME_STYLE)
{
	// notify wxAUI which frame to use
	m_mgr.SetManagedWindow(this);

	CMemoryTree* memTree = new CMemoryTree(this);
	CLogWindow *logWnd = new CLogWindow(this);
	CPropertyWindow *propWnd = new CPropertyWindow(this);
	wxTextCtrl *text3 = new wxTextCtrl(this, wxID_ANY, wxT(" "), 
		wxDefaultPosition, wxSize(200,150),  wxTE_READONLY | wxSUNKEN_BORDER);

	// add the panes to the manager
	m_mgr.AddPane(memTree, wxLEFT, wxT("Memory Tree"));
	m_mgr.AddPane(propWnd, wxRIGHT, wxT("Properties"));
	m_mgr.AddPane(logWnd, wxBOTTOM, wxT("Log Window"));
	m_mgr.AddPane(text3, wxCENTER);

	// tell the manager to "commit" all the changes just made
	m_mgr.Update();

	m_pMemTree = memTree;
	m_pLogWnd = logWnd;
	m_pPropWnd = propWnd;	
}

CFrame::~CFrame()
{
	// deinitialize the frame manager
	m_mgr.UnInit();
}

