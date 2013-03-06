//------------------------------------------------------------------------
// Name:    Log.h
// Author:  jjuiddong
// Date:    1/5/2013
// 
// 로그 기능
//------------------------------------------------------------------------
#pragma once

namespace common { namespace log {

 	void Log(const std::string &str);
	void Log( const char* fmt, ...);

}}
