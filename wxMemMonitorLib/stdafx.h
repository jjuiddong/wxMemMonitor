// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE // warning elimination (strcpy -> strcpy_s )

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.



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

