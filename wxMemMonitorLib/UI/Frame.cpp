
#include "stdafx.h"
#include "Frame.h"
#include "MemoryTree.h"

using namespace memmonitor;


CFrame::CFrame(wxWindow* parent) : wxFrame(parent, -1, _("wxAUI Test"),
	wxDefaultPosition, wxSize(800,600),
	wxDEFAULT_FRAME_STYLE)
{
	// notify wxAUI which frame to use
	m_mgr.SetManagedWindow(this);

	// create several text controls
	//wxTextCtrl* text1 = new wxTextCtrl(this, -1, _("Pane 1 - sample text"),
	//	wxDefaultPosition, wxSize(200,150),
	//	wxNO_BORDER | wxTE_MULTILINE);
	CMemoryTree* text1 = new CMemoryTree(this);
	text1->Init();

	wxTextCtrl* text2 = new wxTextCtrl(this, -1, _("Pane 2 - sample text"),
		wxDefaultPosition, wxSize(200,150),
		wxNO_BORDER | wxTE_MULTILINE);

	wxTextCtrl* text3 = new wxTextCtrl(this, -1, _("Main content window"),
		wxDefaultPosition, wxSize(200,150),
		wxNO_BORDER | wxTE_MULTILINE);

	// add the panes to the manager
	m_mgr.AddPane(text1, wxLEFT, wxT("Memory Tree"));
	m_mgr.AddPane(text2, wxBOTTOM, wxT("Pane Number Two"));
	m_mgr.AddPane(text3, wxCENTER);

	// tell the manager to "commit" all the changes just made
	m_mgr.Update();
}

CFrame::~CFrame()
{
	// deinitialize the frame manager
	m_mgr.UnInit();
}

