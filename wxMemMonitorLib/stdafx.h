// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE // warning elimination (strcpy -> strcpy_s )

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.



/////////////////////////////////////////////////////////
#define ASSERT( x )		assert( (x) )


#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if (p) { delete p; p=NULL;} }
#endif
#ifndef SAFE_DELETEA
#define SAFE_DELETEA(p) {if (p) { delete[] p; p=NULL;} }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#define RET(exp)		{if((exp)) return; }			// exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }


#include <list>
#include <map>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include "dia2.h"
#include <atlbase.h>
#include <comutil.h>
#include <algorithm>
#include <comdef.h> // for _variant_t
#include <boost/foreach.hpp>

#pragma warning(disable: 4355) // disable warning, using this pointer in constructor 

#define  BOOST_DATE_TIME_NO_LIB // boost/interprocess

#undef wxUSE_CMDLINE_PARSER

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "common/String.h"
#include "common/Time.h"
#include "common/Log.h"
#include "common/Reference.h"
#include "common/Singleton.h"


namespace memmonitor
{
	class CMemoryTree;
	class CLogWindow;
	class CPropertyWindow;
	class CFrame;

	typedef ReferencePtr<CMemoryTree> MemTreePtr;
	typedef ReferencePtr<CLogWindow> LogWindowPtr;
	typedef ReferencePtr<CPropertyWindow> PropWindowPtr;
	typedef ReferencePtr<CFrame> FramePtr;

}

#include "wxMemMonitor.h"
