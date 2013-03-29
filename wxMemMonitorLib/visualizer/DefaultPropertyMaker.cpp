
#include "stdafx.h"
#include "DefaultPropertyMaker.h"
#include "PropertyMaker.h"
#include "../dia/DiaWrapper.h"
#include "../ui/PropertyWindow.h"
#include "../ui/PropertyItemAdapter.h"
#include "VisualizerDefine.h"
#include <boost/interprocess/streams/bufferstream.hpp>


namespace visualizer
{
	using namespace parser;

	CPropertyWindow *g_pProperty = NULL;

	// make property
	void		MakeProperty_Root(wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	bool		MakeProperty_Child(  wxPGProperty *pParentProp,  const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_UDTChild(wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_BaseClass(wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_Pointer(wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_Data(wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_Array(wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth );

	wxPGProperty* MakeProperty_BaseType(wxPGProperty *pParentProp, 
		const std::string valueName,  const SSymbolInfo &symbol );

	wxPGProperty* MakeProperty_PointerData(wxPGProperty *pParentProp, 
		const SSymbolInfo &symbol );

 	wxPGProperty* MakeProperty_ArrayData(wxPGProperty *pParentProp, 
 		const SSymbolInfo &symbol );

	wxPGProperty* MakeProperty_UDTData(wxPGProperty *pParentProp, 
		const SSymbolInfo &symbol );

	wxPGProperty* MakeProperty_BaseClassData(wxPGProperty *pParentProp, 
		const SSymbolInfo &symbol);

	void		AddProperty(wxPGProperty *pParentProp, 
		wxPGProperty *prop,  const SSymbolInfo *pSymbol, STypeData *pTypeData);

	_variant_t GetValue(IDiaSymbol *pSymbol, void *pMemPtr);

}

using namespace dia;
using namespace std;
using namespace visualizer;
using namespace memmonitor;

//------------------------------------------------------------------------
// 기본형식으로 property창에 심볼을 출력한다.
// symbolName : 공유메모리에 저장된 심볼이름
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm( CPropertyWindow *pProperties,  const string &symbolName )
{
	const std::string str = ParseObjectName(symbolName);

	IDiaSymbol *pSymbol = dia::FindType(str);
	RETV(!pSymbol, false);

	SMemInfo memInfo;
	if (!FindMemoryInfo(symbolName, memInfo))
	{
		pSymbol->Release();
		return false;
	}

	memInfo.name = symbolName;
	MakeProperty_DefaultForm(pProperties, NULL, SSymbolInfo(pSymbol, memInfo));
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertyWindow *pProperties, 
								 wxPGProperty *pParentProp,  const string &symbolName )
{

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertyWindow *pProperties,
				wxPGProperty *pParentProp,  
				 const SSymbolInfo &symbol )
{
	g_pProperty = pProperties;
	MakeProperty_Root(pParentProp, symbol, 2);
	return true;
}


//------------------------------------------------------------------------
//  symbol.pSym 의 자식을 pParentProp에 추가한다.
//------------------------------------------------------------------------
bool	 visualizer::MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, 
												   wxPGProperty *pParentProp,  const SSymbolInfo &symbol)
{
	g_pProperty = pProperties;
	const bool isVisualizerType = visualizer::MakeVisualizerProperty( pProperties, pParentProp, symbol );
	if (!isVisualizerType)
		MakeProperty_Child(pParentProp, symbol, 2);
	return true;
}


//------------------------------------------------------------------------
//  symbol을 pParentProp의 자식으로 추가한다.
//------------------------------------------------------------------------
bool	 visualizer::MakeProperty_Child(  wxPGProperty *pParentProp,  
									   const SSymbolInfo &symbol, const int depth )
{
	if (depth <= 0)
		return true;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return true;

	switch (symtag)
	{
	case SymTagArrayType:
		MakeProperty_Array(pParentProp, symbol, depth);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer(pParentProp, symbol, depth);
		break;

	case SymTagBaseClass:
		MakeProperty_BaseClass(pParentProp, symbol, depth);
		break;

	case SymTagUDT:
		MakeProperty_UDTChild(pParentProp, symbol, depth);
		break;

	case SymTagData: 
		{
			IDiaSymbol *pBaseType;
			HRESULT hr = symbol.pSym->get_type(&pBaseType);
			ASSERT_RETV(S_OK == hr, false);
			MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType,symbol.mem), depth);
		}
		break;

	case SymTagTypedef:
	case SymTagEnum:
	default:
		break;
	}

	return true;
}


//------------------------------------------------------------------------
// Property 생성
//------------------------------------------------------------------------
void visualizer::MakeProperty_Root(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
								   const int depth )
{
	if (depth <= 0)
		return;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return;

	wxPGProperty *pProp = NULL;
	switch (symtag)
	{
	case SymTagData: 
		MakeProperty_Data(pParentProp, symbol, depth);
		break;

	case SymTagArrayType:
		MakeProperty_Array(pParentProp, symbol, depth);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer(pParentProp, symbol, depth);
		break;

	case SymTagBaseClass:
		pProp = MakeProperty_BaseClassData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, symbol, depth-1);
		break;

	case SymTagUDT:
		pProp = MakeProperty_UDTData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, symbol, depth);
		break;

	case SymTagTypedef:
		break;

	case SymTagEnum:
		break;

	default:
		break;
	}
}


//------------------------------------------------------------------------
// 기본 클래스 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_BaseClass(wxPGProperty *pParentProp, 
											const SSymbolInfo &symbol, const int depth )
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	LocationType locType;
	const LONG offset = dia::GetSymbolLocation(symbol.pSym, &locType);
	BYTE *ptr = (BYTE*)symbol.mem.ptr + offset;
	SMemInfo newMemInfo(symbol.mem.name.c_str(), ptr, 0);
	MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType, newMemInfo), depth);
}


/**
@brief  baseclass type preview
*/
wxPGProperty* visualizer::MakeProperty_BaseClassData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol)
{
	CPropertyItemAdapter prop( symbol.mem.name );
	AddProperty(pParentProp, prop.GetProperty(), &symbol, &STypeData(SymTagBaseClass, VT_EMPTY, NULL));
	return prop.GetProperty();
}


//------------------------------------------------------------------------
// User Define Type 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_UDTChild(wxPGProperty *pParentProp, 
										const SSymbolInfo &symbol, const int depth)
{
	RET (!pParentProp);

	if (depth > 0)
	{
		CComPtr<IDiaEnumSymbols> pEnumChildren;
		if (SUCCEEDED(symbol.pSym->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
		{
			IDiaSymbol *pChild;
			ULONG celt = 0;
			while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
			{
				LONG offset = dia::GetSymbolLocation(pChild);
				SMemInfo memberMemInfo;
				memberMemInfo.name = dia::GetSymbolName(pChild);
				memberMemInfo.ptr = (BYTE*)symbol.mem.ptr + offset;
				MakeProperty_Root(pParentProp, SSymbolInfo(pChild, memberMemInfo), depth-1);
			}
		}
	}
	pParentProp->SetExpanded(false);
	//pParentProp->Expand(FALSE);
}


//------------------------------------------------------------------------
// Pointer 타입 출력 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_Pointer(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
										  const int depth)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
	void *newPtr = MemoryMapping(srcPtr);

	wxPGProperty *pProp = NULL;
	if (SymTagUDT == baseSymTag)
	{
		if (newPtr) 
		{
			SMemInfo ptrMemInfo(symbol.mem.name.c_str(), newPtr, (size_t)0);
			MakeProperty_Root(pParentProp, SSymbolInfo(pBaseType, ptrMemInfo, false), depth);
		}
	}
	pBaseType->Release();
}


//------------------------------------------------------------------------
// Property창에 Control을 추가한다.
// 변수 이름과 타입, 값을 설정한다.
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Data(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
									const int depth)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);
	ASSERT_RET(hr == S_OK);

	wxPGProperty *pPgProp = NULL;
	switch (baseSymTag)
	{
	case SymTagBaseType:
		{
			string valueTypeName = symbol.mem.name;// + " (" + dia::GetSymbolTypeName(symbol.pSym) + ")";
			MakeProperty_BaseType( pParentProp, valueTypeName, symbol);
		}
		break;

	case SymTagEnum:
		{
			std::string typeName = dia::GetSymbolTypeName(pBaseType);
			std::string valueTypeName =  symbol.mem.name;// + " (" +  typeName + ")";

			CPropertyItemAdapter prop( valueTypeName,  CPropertyItemAdapter::PROPTYPE_ENUM ); 
			AddProperty(pParentProp, prop.GetProperty(), &symbol, &STypeData(baseSymTag, VT_UI4, symbol.mem.ptr));

			CComPtr<IDiaEnumSymbols> pEnumChildren;
			IDiaSymbol *pChild;
			if (SUCCEEDED(pBaseType->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
			{
				//int enum_v = 0;
				ULONG celt = 0;
				while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
				{
					std::string e_name = dia::GetSymbolName(pChild);
					prop.AddChoice(e_name);
					pChild->Release();
				}
			}

			ULONGLONG length = 0;
			HRESULT hr = pBaseType->get_length(&length);
			_variant_t value = dia::GetValueFromAddress( symbol.mem.ptr, btUInt, length );
			prop.SetValue( value.intVal );
		}
		break;

	case SymTagUDT:
		pPgProp = MakeProperty_UDTData(pParentProp, symbol);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), depth-1);
		break;

	case SymTagArrayType:
		pPgProp = MakeProperty_ArrayData(pParentProp, symbol);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), depth-1);
		break;

	case SymTagPointerType:
		pPgProp = MakeProperty_PointerData(pParentProp, symbol);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), depth-1);
		break;

	default:
		pBaseType->Release();
		break;
	}
}


//------------------------------------------------------------------------
// Pointer type Preview
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_ArrayData(wxPGProperty *pParentProp, 
												 const SSymbolInfo &symbol )
{
	wxPGProperty *pProp = NULL;

	CComPtr<IDiaSymbol> pArrayType;
	HRESULT hr = symbol.pSym->get_type(&pArrayType); // ArrayDataType
	ASSERT_RETV(S_OK == hr, pProp);

	CComPtr<IDiaSymbol> pElementType;	// 배열 개개의 타입
	hr = pArrayType->get_type(&pElementType);
	ASSERT_RETV(S_OK == hr, pProp);

	enum SymTagEnum elemSymTag;
	hr = pElementType->get_symTag((DWORD*)&elemSymTag);
	assert(S_OK == hr);

	const string typeName = dia::GetSymbolTypeName(symbol.pSym);
	stringstream ss;

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag) // BaseType Array
	{
		BasicType btype;
		hr = pElementType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(S_OK == hr, pProp);

		// char*, char[] 타입이라면 스트링을 출력한다.
		if (btChar == btype)
			ss << symbol.mem.name << " {\"" << (char*)symbol.mem.ptr << "\"}";
		else
			ss << symbol.mem.name << " " << symbol.mem.ptr; // << " (" << typeName << ")";
	}
	else // UDT Array
	{
		ss << symbol.mem.name; // << " (" << typeName << ")";
	}

	CPropertyItemAdapter prop( ss.str() );
	//pProp->SetValue( wxVariant(ss.str(), ss.str()) );
	AddProperty(pParentProp, prop.GetProperty(), &symbol,  &STypeData(SymTagArrayType, VT_EMPTY, NULL) );

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// Pointer Type Preview 
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_PointerData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol )
{
	wxPGProperty *pProp = NULL;

	CComPtr<IDiaSymbol> pPointerType;
	HRESULT hr = symbol.pSym->get_type(&pPointerType);
	ASSERT_RETV(hr == S_OK, pProp);  // PointerType

	CComPtr<IDiaSymbol> pBaseType;
	hr = pPointerType->get_type(&pBaseType);
	ASSERT_RETV(hr == S_OK, pProp);  // BasicDataType or UDTDataType

	void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
	void *newPtr = MemoryMapping(srcPtr);
	if (!newPtr)
		newPtr = srcPtr; // 공유메모리에 없는 데이타일경우 주소만 출력한다.

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	const string name = dia::GetSymbolName(symbol.pSym); // debug용
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);
	stringstream ss;

	if (SymTagUDT == baseSymTag)
	{
		ss << symbol.mem.name;// << " 0x" << newPtr;
		ss << (char*)(CheckValidAddress(newPtr)? " " : " not shared memory");
		//ss << " (" << typeName << ")";
	}
	else if (SymTagBaseType == baseSymTag)
	{
		BasicType btype;
		hr = pBaseType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(hr == S_OK, pProp);

		// char* 타입이라면 스트링을 출력한다.
		if (btChar == btype)
		{
			ss << symbol.mem.name;// << " 0x" << newPtr << " {\"";
			ss << (char*)(CheckValidAddress(newPtr)? newPtr : " not shared memory")  << "\"}";
		}
	}

	if (ss.str().empty()) // default pointer 작업
	{
		ss << symbol.mem.name;// << " 0x" << newPtr;
		ss << (char*)(CheckValidAddress(newPtr)? " " : " not shared memory");
		//ss << " (" << typeName << ")";
	}

	CPropertyItemAdapter prop( ss.str() );
	AddProperty( pParentProp, prop.GetProperty(), &symbol, &STypeData(SymTagPointerType, VT_EMPTY, NULL));

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// UDT type Preview
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_UDTData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol )
{
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);

	// 최상위 UDT가 아닐때만 타입을 출력한다.
	stringstream ss;
	ss << symbol.mem.name;
	//if (pParentProp)
	//	ss << "  (" << typeName << ")";

	CPropertyItemAdapter prop( ss.str());//,  CPropertyItemAdaptor::PROPERTY_PARENT);
	AddProperty(pParentProp, prop.GetProperty(), &symbol, &STypeData(SymTagUDT, VT_EMPTY, symbol.mem.ptr));

	const bool isVisualizerType = visualizer::MakeVisualizerProperty( g_pProperty, 
		prop.GetProperty(), symbol );//  symbol.mem,  symbol.mem.name);
	
	return (isVisualizerType)? NULL : prop.GetProperty();
}


//------------------------------------------------------------------------
// pSymbol : Array Type을 가리킨다. 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Array(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
									 const int depth)
{
	ULONGLONG length=0;
	HRESULT hr = symbol.pSym->get_length(&length);
	ASSERT_RET(S_OK == hr);

	IDiaSymbol* pElementType;	// 배열 개개의 타입
	hr = symbol.pSym->get_type(&pElementType);
	ASSERT_RET(S_OK == hr);

	ULONGLONG element_length=0;
	hr = pElementType->get_length(&element_length);
	ASSERT(S_OK == hr);

	enum SymTagEnum elemSymTag;
	hr = pElementType->get_symTag((DWORD*)&elemSymTag);
	ASSERT(S_OK == hr);

	char valueName[ 128];
	using boost::interprocess::basic_bufferstream;
	basic_bufferstream<char> formatter(valueName, sizeof(valueName));

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag)
	{
		for (ULONGLONG i=0; i < length; i += element_length)
		{
			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;

			void *ptr = (BYTE*)symbol.mem.ptr + i;
			SMemInfo arrayElem(valueName, ptr, (size_t)element_length);
			MakeProperty_BaseType(pParentProp, valueName, SSymbolInfo(pElementType, arrayElem, false));
		}
		//pParentProp->Expand(FALSE); // 일단 접어놓는다.
		pParentProp->SetExpanded(false);
	}
	else // UDT Array
	{
		for (ULONGLONG i=0; i < length; i += element_length)
		{
			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;

			void *ptr = (BYTE*)symbol.mem.ptr + i;
			SMemInfo arrayElem(valueName, ptr, (size_t)element_length);
			SSymbolInfo arraySymbol(pElementType, arrayElem, false);

 			CPropertyItemAdapter prop( valueName );
			AddProperty( pParentProp, prop.GetProperty(), &arraySymbol, &STypeData(SymTagUDT,VT_EMPTY,NULL));

			MakeProperty_Root(prop.GetProperty(), arraySymbol, depth);
		}
		//pParentProp->Expand(FALSE); // 일단 접어놓는다.
		pParentProp->SetExpanded(false);
	}
	pElementType->Release();
}


//------------------------------------------------------------------------
// m_wndPropList 에 Row 를 추가한다.
// pSymbol 은 데이타를 가르키는 심볼이어야 한다.
//------------------------------------------------------------------------
wxPGProperty* visualizer ::MakeProperty_BaseType(
	wxPGProperty *pParentProp, const std::string valueName, 
	const SSymbolInfo &symbol )
{
	_variant_t value = dia::GetValueFromSymbol(symbol.mem.ptr, symbol.pSym);

	CPropertyItemAdapter prop( valueName, symbol, value );
	AddProperty(pParentProp, prop.GetProperty(), &symbol, 
		&STypeData(SymTagData, (prop.IsEnabled()? value.vt : VT_EMPTY), symbol.mem.ptr));

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// Property추가
//------------------------------------------------------------------------
void visualizer ::AddProperty(
								 wxPGProperty *pParentProp, 
								 wxPGProperty *prop, 
								 const SSymbolInfo *pSymbol,
								 STypeData *typeData)
{
	RET(!prop);
	RET(!g_pProperty);

	g_pProperty->AddProperty(pParentProp, prop,  pSymbol, typeData);
}
