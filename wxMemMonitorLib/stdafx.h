// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#define _CRT_SECURE_NO_DEPRECATE // warning elimination (strcpy -> strcpy_s )

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.



/////////////////////////////////////////////////////////
#define ASSERT( x )		assert( (x) )

#include "dia2.h"
#include <atlbase.h>

#undef wxUSE_CMDLINE_PARSER

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "../Common/Common.h"


namespace memmonitor
{
	class CMemoryTree;
	class CLogWindow;
	class CPropertyWindow;
	class CFrame;

	typedef common::ReferencePtr<CMemoryTree> MemTreePtr;
	typedef common::ReferencePtr<CLogWindow> LogWindowPtr;
	typedef common::ReferencePtr<CPropertyWindow> PropWindowPtr;
	typedef common::ReferencePtr<CFrame> FramePtr;

}

#include "wxMemMonitor.h"
