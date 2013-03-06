
#include "stdafx.h"
#include "String.h"


using namespace common;



//------------------------------------------------------------------------
// �����ڵ带 ��Ƽ����Ʈ ���ڷ� ��ȯ
//------------------------------------------------------------------------
std::string common::wstr2str(const std::wstring &wstr)
{
// 	std::locale const& loc = std::locale();
// 	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
// 	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
// 	std::mbstate_t state = 0;
// 	std::vector<char> buf((wstr.size() + 1) * codecvt.max_length());
// 	wchar_t const* in_next = wstr.c_str();
// 	char* out_next = &buf[0];
// 	codecvt_t::result r = codecvt.out(state, 
// 		wstr.c_str(), wstr.c_str() + wstr.size(), in_next, 
// 		&buf[0], &buf[0] + buf.size(), out_next);
// 	return std::string(&buf[0]);

	const int slength = (int)wstr.length() + 1;
	const int len = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, 0, 0, NULL, FALSE);
	char* buf = new char[len];
	::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, buf, len, NULL, FALSE);
	std::string r(buf);
	delete[] buf;
	return r;
}


//------------------------------------------------------------------------
// ��Ƽ����Ʈ ���ڸ� �����ڵ�� ��ȯ
//------------------------------------------------------------------------
std::wstring common::str2wstr(const std::string &str)
{
// 	std::locale const& loc = std::locale();
// 	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;
// 	codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
// 	std::mbstate_t state = 0;
// 	std::vector<wchar_t> buf(str.size() + 1);
// 	char const* in_next = str.c_str();
// 	wchar_t* out_next = &buf[0];
// 	codecvt_t::result r = codecvt.in(state, 
// 		str.c_str(), str.c_str() + str.size(), in_next, 
// 		&buf[0], &buf[0] + buf.size(), out_next);
// 	return std::wstring(&buf[0]);

	int len;
	int slength = (int)str.length() + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}


//------------------------------------------------------------------------
// _variant_t Ÿ���� ��Ʈ������ ��ȯ��Ų��. ����Ÿ ��¿��� ���������.
//------------------------------------------------------------------------
std::string common::variant2str(const _variant_t &var)
{
	std::stringstream ss;
	switch (var.vt)
	{
	case VT_I2: ss << var.iVal; break;
	case VT_I4: ss << var.lVal; break;
	case VT_R4: ss << var.fltVal; break;
	case VT_R8: ss << var.dblVal; break;

	case VT_BSTR:
		{
			tstring str = (LPCTSTR) (_bstr_t)var.bstrVal;
#ifdef _UNICODE
			ss << common::wstr2str(str);
#else
			ss << str;
#endif
		}
		break;

	case VT_DECIMAL:
	case VT_I1:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
		break;

	case VT_INT: ss << var.intVal; break;
	case VT_UINT: ss << var.uintVal; break;
	default:
		break;
	}

	return ss.str();
}


//------------------------------------------------------------------------
// ��Ʈ������
//------------------------------------------------------------------------
std::string common::format(const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );
	return textString;
}


//------------------------------------------------------------------------
// ��Ʈ������ wstring ��
//------------------------------------------------------------------------
std::wstring common::formatw(const char* fmt, ...)
{
	char textString[ 256] = {'\0'};
	va_list args;
	va_start ( args, fmt );
	vsnprintf_s( textString, sizeof(textString), _TRUNCATE, fmt, args );
	va_end ( args );
	return str2wstr(textString);
}
