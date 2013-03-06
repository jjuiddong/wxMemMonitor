
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "Frame.h"

using namespace memmonitor;


bool CApp::OnInit()
{
	if ( !wxApp::OnInit() )
		return false;
	SetExitOnFrameDelete(true);

	wxFrame* frame = new CFrame(NULL);
	SetTopWindow(frame);
	frame->Show();
	return true;
}

void CApp::ExitMainLoop()
{
	// instead of existing wxWidgets main loop, terminate the MFC one
	::PostQuitMessage(0);
}

