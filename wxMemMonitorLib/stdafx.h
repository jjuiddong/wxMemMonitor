// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE // warning elimination (strcpy -> strcpy_s )

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.



/////////////////////////////////////////////////////////
#include "dia2.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "../Common/Common.h"

#define ASSERT( x )		assert( (x) )


struct STypeData
{
	enum SymTagEnum symtag;
	VARTYPE vt;
	void *ptr;
	STypeData( enum SymTagEnum _symtag,  VARTYPE _vt, void *_ptr) : 
	symtag(_symtag), vt(_vt), ptr(_ptr) { }
	STypeData() {}
};

