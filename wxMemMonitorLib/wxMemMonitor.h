//------------------------------------------------------------------------
// Name:    wxMemMonitor.h
// Author:  jjuiddong
// Date:    3/5/2013
// 
// memory monitor library header file
//------------------------------------------------------------------------
#pragma once

#define _AFX_SECURE_NO_WARNINGS 
#define _ATL_SECURE_NO_WARNINGS

#pragma warning (disable: 4996)	// strcpy 경고 제거

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// using memory monitor inner process
#define MEMORYMONITOR_INNER_PROCESS()			\
	DECLARE_APP(memmonitor::CApp)							\
	IMPLEMENT_APP_NO_MAIN(memmonitor::CApp)		\


// generate memory monitor execute file
#define MEMORYMONITOR_OUTER_PROCESS()		\
	DECLARE_APP(memmonitor::CApp)						\
	IMPLEMENT_APP(memmonitor::CApp)					\


namespace memmonitor
{
	enum EXECUTE_TYPE
	{
		INNER_PROCESS,		// using inner process memory
		OUTER_PROCESS,		// using shared memory
	};

	bool Init(EXECUTE_TYPE type, HINSTANCE hInst, const std::string configFileName);
	void Loop(MSG &msg);
	void Cleanup();
	
	// Error Report
	const std::string& GetLastError();

	// App
	class CApp : public wxApp
	{
	public:
		virtual bool OnInit();
		virtual void ExitMainLoop();
	};
}
