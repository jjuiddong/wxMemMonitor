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

		bool AddPropertyWindow(const wxString &symbolName );

		MemTreePtr GetMemTree() const;
		LogWindowPtr GetLogWindow() const;
		PropWindowPtr GetPropWindow() const;

	protected:
		bool	InitMemoryMonitor(const std::string &configFileName);
		bool LoadConfigFile(const std::string &fileName);
		
	private:
		wxAuiManager m_mgr;
		MemTreePtr m_pMemTree;
		LogWindowPtr m_pLogWnd;
		PropWindowPtr m_pPropWnd;
	};

	inline MemTreePtr CFrame::GetMemTree() const { return m_pMemTree; }
	inline LogWindowPtr CFrame::GetLogWindow() const { return m_pLogWnd; }
	inline PropWindowPtr CFrame::GetPropWindow() const { return m_pPropWnd; }

}
