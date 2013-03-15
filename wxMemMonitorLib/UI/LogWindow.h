//------------------------------------------------------------------------
// Name:    LogWindow.h
// Author:  jjuiddong
// Date:    2013-03-07
// 
// 
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	class CLogWindow : public wxTextCtrl
	{
	public:
		CLogWindow(wxWindow *parent);
		virtual ~CLogWindow() {}

		void PrintText( const wxString &msg );

	protected:

	};
}
