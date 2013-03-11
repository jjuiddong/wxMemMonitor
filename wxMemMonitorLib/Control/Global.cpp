
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "Global.h"
#include "../ui/Frame.h"

namespace memmonitor
{
	EXECUTE_TYPE g_Type;
	std::string g_ErrorMsg;
}

using namespace memmonitor;


EXECUTE_TYPE memmonitor::GetExecuteType() 
{ 
	return g_Type; 
}
void memmonitor::SetExecuteType(EXECUTE_TYPE type) 
{ 
	g_Type = type; 
}
const std::string& memmonitor::GetErrorMsg()
{
	return g_ErrorMsg;
}
void	memmonitor::SetErrorMsg(const std::string&msg)
{
	g_ErrorMsg = msg;
}

MemTreePtr memmonitor::GetMemoryTree()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	RETV(!pFrame, NULL);
	return pFrame->GetMemTree();
}
LogWindowPtr	memmonitor::GetLogWindow()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	RETV(!pFrame, NULL);
	return pFrame->GetLogWindow();
}
PropWindowPtr memmonitor::GetPropertyWindow()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	RETV(!pFrame, NULL);
	return pFrame->GetPropWindow();
}
