
#include "stdafx.h"
#include "PropertyGrid.h"

using namespace memmonitor;

CPropertyGrid::CPropertyGrid( std::string label ) :
	wxPGProperty(common::str2wstr(label), wxT(""))
{
}

CPropertyGrid::~CPropertyGrid()
{

}


//------------------------------------------------------------------------
// convert _variant_t -> wxVariant 
// and setting using SetValue(wxVariant&) member function
//------------------------------------------------------------------------
void CPropertyGrid::SetVariantValue( const _variant_t &var )
{
	const wxVariant wxVar = variant2wxVariant(var);
	SetValue(wxVar);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
_variant_t CPropertyGrid::GetVariantValue()
{
	const wxVariant wxVar = GetValue();
	_variant_t var = wxVariant2variant(wxVar);
	return var;
}


//------------------------------------------------------------------------
// wxVariant1 <= variant0; variant2 <= wxVariant1
// variant0 != variant2   
// variant 에서 wxVariant로 변환되면서 타입이 바뀔 수 있다.
//------------------------------------------------------------------------
wxVariant memmonitor::variant2wxVariant(const _variant_t &var)
{
	wxVariant wxVar;
	switch (var.vt)
	{
	case VT_I2: wxVar = wxVariant(var.iVal); break;
	case VT_I4: wxVar = wxVariant(var.lVal); break;
	case VT_R4: wxVar = wxVariant(var.fltVal); break;
	case VT_R8: wxVar = wxVariant(var.dblVal); break;

	case VT_BOOL: wxVar = wxVariant(var.bVal); break;
	case VT_DECIMAL: break;
	case VT_I1: wxVar = wxVariant(var.cVal); break;
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
_variant_t memmonitor::wxVariant2variant(const wxVariant &wxVar)
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
