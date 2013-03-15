
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "Global.h"
#include "../ui/Frame.h"

namespace memmonitor
{
	EXECUTE_TYPE g_Type = OUTER_PROCESS;
	std::string g_ErrorMsg;
	std::string g_ConfigFileName;
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
const std::string& memmonitor::GetConfigFileName()
{
	return g_ConfigFileName;
}
void memmonitor::SetConfigFileName(const std::string &fileName)
{
	g_ConfigFileName = fileName;
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
FramePtr memmonitor::GetFrame()
{
	CFrame *pFrame = static_cast<CFrame*>(wxTheApp->GetTopWindow());
	return pFrame;
}


//------------------------------------------------------------------------
// wxVariant1 <= variant0; variant2 <= wxVariant1
// variant0 != variant2   
// variant 에서 wxVariant로 변환되면서 타입이 바뀔 수 있다.
//------------------------------------------------------------------------
wxVariant memmonitor::Variant2wxVariant(const _variant_t &var)
{
	wxVariant wxVar;
	switch (var.vt)
	{
	case VT_I1: wxVar = (long)var.cVal; break;
	case VT_I2: wxVar = wxVariant(var.iVal); break;
	case VT_I4: wxVar = wxVariant(var.lVal); break;
	case VT_R4: wxVar = wxVariant(var.fltVal); break;
	case VT_R8: wxVar = wxVariant(var.dblVal); break;

	case VT_BOOL: wxVar = wxVariant(var.bVal); break;
	case VT_DECIMAL: break;

	case VT_UI1: wxVar = wxVariant(var.bVal); break;
	case VT_UI2: wxVar = wxVariant(var.uiVal); break;
	case VT_UI4: wxVar = wxVariant((int)var.ulVal); break;
	case VT_I8: wxVar = wxVariant(wxLongLong(var.llVal)); break;
	case VT_UI8: wxVar = wxVariant(wxULongLong(var.ullVal)); break;

	case VT_INT: wxVar = wxVariant(var.intVal); break;
	case VT_UINT: wxVar = wxVariant((int)var.uintVal); break;

	case VT_BSTR:
		{
			//			std::string str;
			//			packet  >>(str);
			//#ifdef _UNICODE
			//			varType.bstrVal = (_bstr_t)common::str2wstr(str).c_str();
			//#else
			//			varType.bstrVal = (_bstr_t)str.c_str();
			//#endif
		}
		break;
	default:
		break;
	}
	return wxVar;
}


//------------------------------------------------------------------------
// variant <= wxVariant
//------------------------------------------------------------------------
_variant_t memmonitor::wxVariant2Variant(const wxVariant &wxVar)
{
	_variant_t var;
	const wxString type = wxVar.GetType();
	if (type == "bool")
	{
		var = _variant_t(wxVar.GetBool());
	}
	else if (type == "char")
	{
		var = _variant_t((char)wxVar);
	}
	else if (type == "datetime")
	{
	}
	else if (type == "double")
	{
		var = _variant_t(wxVar.GetDouble());
	}
	else if (type == "list")
	{
	}
	else if (type == "long")
	{
		var = _variant_t(wxVar.GetLong());
	}
	else if (type == "longlong")
	{
	}
	else if (type == "string")
	{
	}
	else if (type == "ulonglong")
	{
	}
	else if (type == "arrstring")
	{
	}
	else if (type == "void*")
	{
	}
	return var;
}


//------------------------------------------------------------------------
// convert wxVariant to _variant_t with vartype format 
//------------------------------------------------------------------------
_variant_t memmonitor::wxVariant2Variant(const VARTYPE &vartype, const wxVariant &wxVar)
{
	_variant_t tmp = wxVariant2Variant(wxVar);

	_variant_t var;	
	switch (vartype)
	{
	case VT_I1: var = (char)tmp; break;
	case VT_I2: var = (SHORT)tmp; break;
	case VT_I4: var = (long)tmp; break;
	case VT_I8: var = (LONGLONG)tmp; break;
	case VT_UI1: var = (BYTE)tmp; break;
	case VT_UI2: var = (u_short)tmp; break;
	case VT_UI4: var = (DWORD)tmp; break;
	case VT_UI8: var = (ULONGLONG)tmp; break;	
	case VT_R4: var = (float)tmp; break;
	case VT_R8: var = (double)tmp; break;
	case VT_INT: var = (int)tmp; break;
	case VT_UINT: var = (unsigned int)tmp; break;

	case VT_BOOL: var = (bool)tmp; break;
	case VT_DECIMAL: break;

	case VT_BSTR:
		break;
	default:
		break;
	}
	return var;
}
