
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

	// make property
	void		MakeProperty_Root(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
		const bool IsUdtExpand, const int depth  );

	bool		MakeProperty_Child(  wxPGProperty *pParentProp,  const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const int depth );

	void		MakeProperty_UDTChild(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const int depth );

	void		MakeProperty_BaseClass(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const int depth );

	void		MakeProperty_Pointer(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const int depth );

	void		MakeProperty_Data(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const int depth );

	void		MakeProperty_Array(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
		const bool IsUdtExpand, const int depth );


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

	wxPGProperty*  AddProperty(wxPGProperty *pParentProp, 
		CPropertyItemAdapter &propAdapter,  const SSymbolInfo *pSymbol, 
		STypeData *pTypeData);

	_variant_t GetValue(IDiaSymbol *pSymbol, void *pMemPtr);

	// variables
	CPropertyWindow *g_pProperty = NULL;
	bool g_IsApplyVisualizer = true;
}

using namespace dia;
using namespace std;
using namespace visualizer;
using namespace memmonitor;

//------------------------------------------------------------------------
// �⺻�������� propertyâ�� �ɺ��� ����Ѵ�.
// symbolName : �ɺ� �̸�  {symbolname#count}
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm( CPropertyWindow *pProperties,  const string &symbolName )
{
	return MakeProperty_DefaultForm(pProperties, NULL, symbolName);
}


/**
@brief  propertyâ�� �ɺ��� ����Ѵ�.
@param symbolName : {symbolName#count} �޸� ������ ���� �� �ִ� ����
*/
bool	visualizer::MakeProperty_DefaultForm( CPropertyWindow *pProperties,  wxPGProperty *pParentProp,
	const std::string &symbolName )
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
	MakeProperty_DefaultForm(pProperties, pParentProp, SSymbolInfo(pSymbol, memInfo));
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertyWindow *pProperties,
				wxPGProperty *pParentProp,  const SSymbolInfo &symbol,
				bool IsApplyVisualizer ) // IsApplyVisualizer = true
{
	if (!symbol.mem.ptr)
		return true;

	g_pProperty = pProperties;
	g_IsApplyVisualizer = IsApplyVisualizer;
	MakeProperty_Root(pParentProp, symbol, true, 2);
	return true;
}


//------------------------------------------------------------------------
//  symbol.pSym �� �ڽ��� pParentProp�� �߰��Ѵ�.
//------------------------------------------------------------------------
bool	 visualizer::MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, 
												   wxPGProperty *pParentProp,  const SSymbolInfo &symbol, 
												   bool IsApplyVisualizer) // , IsApplyVisualizer = true
{
	if (!symbol.mem.ptr)
		return true;

	g_pProperty = pProperties;
	g_IsApplyVisualizer = IsApplyVisualizer;

	bool isVisualizerType = false;
	if (g_IsApplyVisualizer)
		isVisualizerType = visualizer::MakeVisualizerProperty( pProperties, pParentProp, symbol );

	if (!isVisualizerType)
		MakeProperty_Child(pParentProp, symbol, true, 2);
	return true;
}


//------------------------------------------------------------------------
//  symbol�� pParentProp�� �ڽ����� �߰��Ѵ�.
//------------------------------------------------------------------------
bool	 visualizer::MakeProperty_Child(  wxPGProperty *pParentProp,  const SSymbolInfo &symbol, 
	const bool IsUdtExpand, const int depth )
{
	if (depth <= 0)
		return true;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return true;

	switch (symtag)
	{
	case SymTagArrayType:
		MakeProperty_Array(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagBaseClass:
		MakeProperty_BaseClass(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagUDT:
		MakeProperty_UDTChild(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagData: 
		{
			IDiaSymbol *pBaseType;
			HRESULT hr = symbol.pSym->get_type(&pBaseType);
			ASSERT_RETV(S_OK == hr, false);
			MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType,symbol.mem), 
				IsUdtExpand, depth);
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
// Property ����
//------------------------------------------------------------------------
void visualizer::MakeProperty_Root(wxPGProperty *pParentProp, const SSymbolInfo &symbol, 
	const bool IsUdtExpand, const int depth )
{
	if (depth <= 0)
		return;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return;

	wxPGProperty *pProp = NULL;
	switch (symtag)
	{
	case SymTagBaseType:
		MakeProperty_BaseType(pParentProp, symbol.mem.name, symbol);
		break;

	case SymTagData: 
		MakeProperty_Data(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagArrayType:
		MakeProperty_Array(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagPointerType:
		MakeProperty_Pointer(pParentProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagBaseClass:
		pProp = MakeProperty_BaseClassData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagUDT:
		pProp = MakeProperty_UDTData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, symbol, IsUdtExpand, depth);
		break;

	case SymTagTypedef:
		break;

	case SymTagEnum:
		break;

	default:
		break;
	}
}


/**
 @brief �⺻ Ŭ���� 
 */
void	visualizer ::MakeProperty_BaseClass(wxPGProperty *pParentProp, 
											const SSymbolInfo &symbol, 
											const bool IsUdtExpand, const int depth )
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	LocationType locType;
	const LONG offset = dia::GetSymbolLocation(symbol.pSym, &locType);
	BYTE *ptr = (BYTE*)symbol.mem.ptr + offset;
	SMemInfo newMemInfo(symbol.mem.name.c_str(), ptr, 0);
	MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType, newMemInfo), IsUdtExpand, depth);
}


/**
@brief  baseclass type preview
*/
wxPGProperty* visualizer::MakeProperty_BaseClassData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol)
{
	CPropertyItemAdapter prop( symbol.mem.name );
	AddProperty(pParentProp, prop, &symbol, 
		&STypeData(SymTagBaseClass, VT_EMPTY, NULL));
	return prop.GetProperty();
}


//------------------------------------------------------------------------
// User Define Type 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_UDTChild(wxPGProperty *pParentProp, 
										const SSymbolInfo &symbol, 
										const bool IsUdtExpand, const int depth)
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
				bool childIsUdtExpand = IsUdtExpand;
				int childDepth = depth - 1;
				if (IsUdtExpand && (depth == 2))
				{
					enum SymTagEnum childSymTag;
					HRESULT hr = pChild->get_symTag((DWORD*)&childSymTag);
					if (SymTagUDT == childSymTag || SymTagBaseClass == childSymTag)
					{
						childIsUdtExpand = false;
						childDepth = 2;
					}
				}

				LONG offset = dia::GetSymbolLocation(pChild);
				SMemInfo memberMemInfo;
				memberMemInfo.name = dia::GetSymbolName(pChild);
				memberMemInfo.ptr = (BYTE*)symbol.mem.ptr + offset;
				MakeProperty_Root(pParentProp, SSymbolInfo(pChild, memberMemInfo), 
					childIsUdtExpand, childDepth);
			}
		}
	}
	pParentProp->SetExpanded(false);
}


//------------------------------------------------------------------------
// Pointer Ÿ�� ��� 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_Pointer(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
										  const bool IsUdtExpand, const int depth)
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
			MakeProperty_Root(pParentProp, SSymbolInfo(pBaseType, ptrMemInfo, false), 
				IsUdtExpand, depth);
		}
	}
	pBaseType->Release();
}


//------------------------------------------------------------------------
// Propertyâ�� Control�� �߰��Ѵ�.
// ���� �̸��� Ÿ��, ���� �����Ѵ�.
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Data(wxPGProperty *pParentProp, const SSymbolInfo &symbol,
									const bool IsUdtExpand, const int depth)
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
			AddProperty(pParentProp, prop, &symbol, &STypeData(baseSymTag, VT_UI4, symbol.mem.ptr));

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
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, depth-1);
		break;

	case SymTagArrayType:
		pPgProp = MakeProperty_ArrayData(pParentProp, symbol);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, depth-1);
		break;

	case SymTagPointerType:
		pPgProp = MakeProperty_PointerData(pParentProp, symbol);
		if (pPgProp)
			MakeProperty_Child(pPgProp, SSymbolInfo(pBaseType, symbol.mem), IsUdtExpand, depth-1);
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

	CComPtr<IDiaSymbol> pElementType;	// �迭 ������ Ÿ��
	hr = pArrayType->get_type(&pElementType);
	ASSERT_RETV(S_OK == hr, pProp);

	enum SymTagEnum elemSymTag;
	hr = pElementType->get_symTag((DWORD*)&elemSymTag);
	assert(S_OK == hr);

	const string typeName = dia::GetSymbolTypeName(symbol.pSym);
	stringstream ss;
	string stringVal;
	ss << symbol.mem.name;

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag) // BaseType Array
	{
		BasicType btype;
		hr = pElementType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(S_OK == hr, pProp);
		
		// char*, char[] Ÿ���̶�� ��Ʈ���� ����Ѵ�.
		if (btChar == btype)
			stringVal = (char*)symbol.mem.ptr;
	}

	// todo : ���߿� preview visualizer �� �����ؾ� �Ѵ�.
	// string Ÿ���� ��, ���
	if (pParentProp)
	{
		CPropertyWindow::SPropItem *pItemData = 
			(CPropertyWindow::SPropItem*)pParentProp->GetClientData();
		if (pItemData)
		{
			if (!strncmp(pItemData->symbolTypeName.c_str(),  "std::basic_string",17 ))
			{
				pParentProp->SetValue( stringVal.c_str() );
			}
		}
	}

	CPropertyItemAdapter prop( ss.str(), CPropertyItemAdapter::PROPERTY_STRING, 0, stringVal );
	AddProperty(pParentProp, prop, &symbol,  &STypeData(SymTagArrayType, VT_EMPTY, NULL) );

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
		newPtr = srcPtr; // �����޸𸮿� ���� ����Ÿ�ϰ�� �ּҸ� ����Ѵ�.

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	const string name = dia::GetSymbolName(symbol.pSym); // debug��
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

		// char* Ÿ���̶�� ��Ʈ���� ����Ѵ�.
		if (btChar == btype)
		{
			ss << symbol.mem.name;// << " 0x" << newPtr << " {\"";
			ss << (char*)(CheckValidAddress(newPtr)? newPtr : " not shared memory")  << "\"}";
		}
	}

	if (ss.str().empty()) // default pointer �۾�
	{
		ss << symbol.mem.name;// << " 0x" << newPtr;
		ss << (char*)(CheckValidAddress(newPtr)? " " : " not shared memory");
		//ss << " (" << typeName << ")";
	}

	CPropertyItemAdapter prop( ss.str(), CPropertyItemAdapter::PROPTYPE_POINTER, (DWORD)newPtr );
	AddProperty( pParentProp, prop, &symbol, &STypeData(SymTagPointerType, VT_EMPTY, NULL));

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// UDT type Preview
//------------------------------------------------------------------------
wxPGProperty* visualizer::MakeProperty_UDTData(
	wxPGProperty *pParentProp, const SSymbolInfo &symbol ) //  IsApplyVisualizer = true
{
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);

	// �ֻ��� UDT�� �ƴҶ��� Ÿ���� ����Ѵ�.
	stringstream ss;
	ss << symbol.mem.name;

	CPropertyItemAdapter prop( ss.str());
	AddProperty(pParentProp, prop, &symbol, &STypeData(SymTagUDT, VT_EMPTY, symbol.mem.ptr));

	bool isVisualizerType = false;
	// todo: visualizer preview �۾��� ������ ���־��Ѵ�.
	if (!strncmp(typeName.c_str(),  "std::basic_string",17 ))
	{
		isVisualizerType = visualizer::MakeVisualizerProperty( g_pProperty, prop.GetProperty(), symbol );
		prop.GetProperty()->SetExpanded(false);
	}
	else if (g_IsApplyVisualizer)
	{
		isVisualizerType = visualizer::MakeVisualizerProperty( g_pProperty, prop.GetProperty(), symbol );
	}
	
	return (isVisualizerType)? NULL : prop.GetProperty();
}


//------------------------------------------------------------------------
// pSymbol : Array Type�� ����Ų��. 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Array(wxPGProperty *pParentProp, 
	const SSymbolInfo &symbol, const bool IsUdtExpand, const int depth)
{
	ULONGLONG length=0;
	HRESULT hr = symbol.pSym->get_length(&length);
	ASSERT_RET(S_OK == hr);

	IDiaSymbol* pElementType;	// �迭 ������ Ÿ��
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
		//pParentProp->Expand(FALSE); // �ϴ� ������´�.
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
			AddProperty( pParentProp, prop, &arraySymbol, &STypeData(SymTagUDT,VT_EMPTY,NULL));

			MakeProperty_Root(prop.GetProperty(), arraySymbol, IsUdtExpand, depth);
		}
		//pParentProp->Expand(FALSE); // �ϴ� ������´�.
		pParentProp->SetExpanded(false);
	}
	pElementType->Release();
}


//------------------------------------------------------------------------
// m_wndPropList �� Row �� �߰��Ѵ�.
// pSymbol �� ����Ÿ�� ����Ű�� �ɺ��̾�� �Ѵ�.
//------------------------------------------------------------------------
wxPGProperty* visualizer ::MakeProperty_BaseType(
	wxPGProperty *pParentProp, const std::string valueName, 
	const SSymbolInfo &symbol )
{
	_variant_t value = dia::GetValueFromSymbol(symbol.mem.ptr, symbol.pSym);

	CPropertyItemAdapter prop( valueName, symbol, value );
	AddProperty(pParentProp, prop, &symbol, 
		&STypeData(SymTagData, (prop.IsEnabled()? value.vt : VT_EMPTY), symbol.mem.ptr));

	return prop.GetProperty();
}


//------------------------------------------------------------------------
// Property�߰�
//------------------------------------------------------------------------
wxPGProperty* visualizer ::AddProperty(
								 wxPGProperty *pParentProp, 
								 CPropertyItemAdapter &propAdapter,
								 const SSymbolInfo *pSymbol,
								 STypeData *typeData)
{
	RETV(!g_pProperty, NULL);

	return g_pProperty->AddProperty(pParentProp, propAdapter,  pSymbol, typeData);
}
