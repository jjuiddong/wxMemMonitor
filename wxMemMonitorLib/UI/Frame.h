//------------------------------------------------------------------------
// Name:    Frame.h
// Author:  jjuiddong
// Date:    3/5/2013
// 
// Basic Frame class
//------------------------------------------------------------------------
#pragma once

#include <wx/aui/aui.h>

namespace memmonitor
{
	class CFrame : public wxFrame 
	{
	public:
		CFrame(wxWindow* parent);
		virtual ~CFrame();

	private:
		wxAuiManager m_mgr;
	};

}
