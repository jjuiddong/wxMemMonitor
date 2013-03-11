//------------------------------------------------------------------------
// Name:    Global.h
// Author:  jjuiddong
// Date:    3/11/2013
// 
// memmonitor global function
//------------------------------------------------------------------------
#pragma once

namespace memmonitor
{
	//const std::string& GetConfigFileName();
	//void SetConfigFileName(const std::string &fileName);


	EXECUTE_TYPE		GetExecuteType();
	void							SetExecuteType(EXECUTE_TYPE type);

	MemTreePtr				GetMemoryTree();
	LogWindowPtr			GetLogWindow();
	PropWindowPtr		GetPropertyWindow();

	const std::string&	GetErrorMsg();
	void							SetErrorMsg(const std::string&msg);

}
