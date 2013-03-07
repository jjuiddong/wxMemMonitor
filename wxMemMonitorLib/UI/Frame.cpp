
#include "stdafx.h"
#include "Frame.h"
#include "MemoryTree.h"
#include "LogWindow.h"

using namespace memmonitor;


CFrame::CFrame(wxWindow* parent) : wxFrame(parent, -1, _("wxAUI Test"),
	wxDefaultPosition, wxSize(800,600),
	wxDEFAULT_FRAME_STYLE)
{
	// notify wxAUI which frame to use
	m_mgr.SetManagedWindow(this);

	CMemoryTree* memTree = new CMemoryTree(this);
	memTree->Init();

	CLogWindow *logWnd = new CLogWindow(this);

	wxTextCtrl *text3 = new wxTextCtrl(this, wxID_ANY, _("Pane 3 - sample text"),
		wxDefaultPosition, wxSize(200,150),
		wxTE_MULTILINE | wxSUNKEN_BORDER);

	// add the panes to the manager
	m_mgr.AddPane(memTree, wxLEFT, wxT("Memory Tree"));
	m_mgr.AddPane(logWnd, wxBOTTOM, wxT("Pane Number Two"));
	m_mgr.AddPane(text3, wxCENTER);

	// tell the manager to "commit" all the changes just made
	m_mgr.Update();
}

CFrame::~CFrame()
{
	// deinitialize the frame manager
	m_mgr.UnInit();
}

