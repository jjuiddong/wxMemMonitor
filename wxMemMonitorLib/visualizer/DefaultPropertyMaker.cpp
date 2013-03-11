
#include "stdafx.h"
#include "DefaultPropertyMaker.h"
#include "PropertyMaker.h"
#include "../dia/DiaWrapper.h"
#include "../ui/PropertyWindow.h"
#include "../ui/PropertyGrid.h"
#include "VisualizerDefine.h"
#include <atlbase.h>


namespace visualizer
{
	using namespace sharedmemory;
	using namespace parser;

	CPropertyWindow *n_pProperty = NULL;

	// make property
	void		MakeProperty_Root(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, const int depth );

	bool		MakeProperty_Child(  CPropertyGrid *pParentProp,  const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_UDTChild(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_BaseClass(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_Pointer(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_Data(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, const int depth );

	void		MakeProperty_Array(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, const int depth );

	CPropertyGrid* MakeProperty_BaseType(CPropertyGrid *pParentProp, 
		const std::string valueName,  const SSymbolInfo &symbol );

	CPropertyGrid* MakeProperty_PointerData(CPropertyGrid *pParentProp, 
		const SSymbolInfo &symbol );

 	CPropertyGrid* MakeProperty_ArrayData(CPropertyGrid *pParentProp, 
 		const SSymbolInfo &symbol );

	CPropertyGrid* MakeProperty_UDTData(CPropertyGrid *pParentProp, 
		const SSymbolInfo &symbol );

	CPropertyGrid* MakeProperty_BaseClassData(CPropertyGrid *pParentProp, 
		const SSymbolInfo &symbol);

	void		AddProperty(CPropertyGrid *pParentProp, 
		CPropertyGrid *prop,  const SSymbolInfo *pSymbol, STypeData *pTypeData);

	_variant_t GetValue(IDiaSymbol *pSymbol, void *pMemPtr);

}

using namespace dia;
using namespace std;
using namespace visualizer;
using namespace memmonitor;

//------------------------------------------------------------------------
// �⺻�������� propertyâ�� �ɺ��� ����Ѵ�.
// symbolName : �����޸𸮿� ����� �ɺ��̸�
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm( CPropertyWindow *pProperties,  const string &symbolName )
{
	const std::string str = sharedmemory::ParseObjectName(symbolName);

	IDiaSymbol *pSymbol = CDiaWrapper::Get()->FindType(str);
	RETV(!pSymbol, false);

	sharedmemory::SMemoryInfo memInfo;
	if (!sharedmemory::FindMemoryInfo(symbolName, memInfo))
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
								 CPropertyGrid *pParentProp,  const string &symbolName )
{

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakeProperty_DefaultForm(  CPropertyWindow *pProperties,
				CPropertyGrid *pParentProp,  
				 const SSymbolInfo &symbol )
{
	n_pProperty = pProperties;
	MakeProperty_Root(pParentProp, symbol, 2);
	return true;
}


//------------------------------------------------------------------------
//  symbol.pSym �� �ڽ��� pParentProp�� �߰��Ѵ�.
//------------------------------------------------------------------------
bool	 visualizer::MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, 
												   CPropertyGrid *pParentProp,  const SSymbolInfo &symbol)
{
	n_pProperty = pProperties;
	const bool isVisualizerType = visualizer::MakeVisualizerProperty( pProperties, pParentProp, symbol );
	if (!isVisualizerType)
		MakeProperty_Child(pParentProp, symbol, 2);
	return true;
}


//------------------------------------------------------------------------
//  symbol�� pParentProp�� �ڽ����� �߰��Ѵ�.
//------------------------------------------------------------------------
bool	 visualizer::MakeProperty_Child(  CPropertyGrid *pParentProp,  
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
// Property ����
//------------------------------------------------------------------------
void visualizer::MakeProperty_Root(CPropertyGrid *pParentProp, const SSymbolInfo &symbol, 
								   const int depth )
{
	if (depth <= 0)
		return;
	enum SymTagEnum symtag;
	if (S_OK != symbol.pSym->get_symTag((DWORD*)&symtag))
		return;

	CPropertyGrid *pProp = NULL;
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
// �⺻ Ŭ���� 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_BaseClass(CPropertyGrid *pParentProp, 
											const SSymbolInfo &symbol, const int depth )
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	LocationType locType;
	const LONG offset = dia::GetSymbolLocation(symbol.pSym, &locType);
	BYTE *ptr = (BYTE*)symbol.mem.ptr + offset;
	SMemoryInfo newMemInfo(symbol.mem.name.c_str(), ptr, 0);
	MakeProperty_Child(pParentProp, SSymbolInfo(pBaseType, newMemInfo), depth);
}

//------------------------------------------------------------------------
// baseclass type preview
//------------------------------------------------------------------------
CPropertyGrid* visualizer::MakeProperty_BaseClassData(
	CPropertyGrid *pParentProp, const SSymbolInfo &symbol)
{
	CPropertyGrid *pProp = new CPropertyGrid( symbol.mem.name );
	AddProperty(pParentProp, pProp, &symbol, &STypeData(SymTagBaseClass, VT_EMPTY, NULL));
	return pProp;
}


//------------------------------------------------------------------------
// User Define Type 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_UDTChild(CPropertyGrid *pParentProp, 
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
				SMemoryInfo memberMemInfo;
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
// Pointer Ÿ�� ��� 
//------------------------------------------------------------------------
void	visualizer ::MakeProperty_Pointer(CPropertyGrid *pParentProp, const SSymbolInfo &symbol,
										  const int depth)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
	void *newPtr = sharedmemory::MemoryMapping(srcPtr);

	CPropertyGrid *pProp = NULL;
	if (SymTagUDT == baseSymTag)
	{
		if (newPtr) 
		{
			SMemoryInfo ptrMemInfo(symbol.mem.name.c_str(), newPtr, (size_t)0);
			MakeProperty_Root(pParentProp, SSymbolInfo(pBaseType, ptrMemInfo, false), depth);
		}
	}
	pBaseType->Release();
}


//------------------------------------------------------------------------
// Propertyâ�� Control�� �߰��Ѵ�.
// ���� �̸��� Ÿ��, ���� �����Ѵ�.
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Data(CPropertyGrid *pParentProp, const SSymbolInfo &symbol,
									const int depth)
{
	IDiaSymbol* pBaseType;
	HRESULT hr = symbol.pSym->get_type(&pBaseType);
	ASSERT_RET(hr == S_OK);

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);
	ASSERT_RET(hr == S_OK);

	CPropertyGrid *pProp;
	switch (baseSymTag)
	{
	case SymTagBaseType:
		{
			string valueTypeName = symbol.mem.name + " (" + dia::GetSymbolTypeName(symbol.pSym) + ")";
			MakeProperty_BaseType( pParentProp, valueTypeName, symbol);
		}
		break;

	case SymTagEnum:
		{
			std::string typeName = dia::GetSymbolTypeName(pBaseType);
			std::string valueTypeName =  symbol.mem.name + " (" +  typeName + ")";
			//pProp = new CPropertyGrid( valueTypeName, _T(" ") ); 
			pProp = new CPropertyGrid( valueTypeName ); 
			AddProperty(pParentProp, pProp, &symbol, &STypeData(baseSymTag, VT_UI4, symbol.mem.ptr));

			CComPtr<IDiaEnumSymbols> pEnumChildren;
			IDiaSymbol *pChild;
			if (SUCCEEDED(pBaseType->findChildren(SymTagNull, NULL, nsNone, &pEnumChildren))) 
			{
				ULONG celt = 0;
				while (SUCCEEDED(pEnumChildren->Next(1, &pChild, &celt)) && (celt == 1)) 
				{
					std::string e_name = dia::GetSymbolName(pChild);
					//pProp->AddOption( common::str2wstr(e_name).c_str() );
					pChild->Release();
				}
			}
			//pProp->AllowEdit(FALSE);
			pProp->SetModifiedStatus(false);

			ULONGLONG length = 0;
			HRESULT hr = pBaseType->get_length(&length);
			_variant_t value = dia::GetValueFromAddress( symbol.mem.ptr, btUInt, length );
			SetPropertyValue( pProp, value);
		}
		break;

	case SymTagUDT:
		pProp = MakeProperty_UDTData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, SSymbolInfo(pBaseType, symbol.mem), depth-1);
		break;

	case SymTagArrayType:
		pProp = MakeProperty_ArrayData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, SSymbolInfo(pBaseType, symbol.mem), depth-1);
		break;

	case SymTagPointerType:
		pProp = MakeProperty_PointerData(pParentProp, symbol);
		if (pProp)
			MakeProperty_Child(pProp, SSymbolInfo(pBaseType, symbol.mem), depth-1);
		break;

	default:
		pBaseType->Release();
		break;
	}
}


//------------------------------------------------------------------------
// Pointer type Preview
//------------------------------------------------------------------------
CPropertyGrid* visualizer::MakeProperty_ArrayData(CPropertyGrid *pParentProp, 
												 const SSymbolInfo &symbol )
{
	CPropertyGrid *pProp = NULL;

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

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag) // BaseType Array
	{
		BasicType btype;
		hr = pElementType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(S_OK == hr, pProp);

		// char*, char[] Ÿ���̶�� ��Ʈ���� ����Ѵ�.
		if (btChar == btype)
			ss << symbol.mem.name << " {\"" << (char*)symbol.mem.ptr << "\"}";
		else
			ss << symbol.mem.name << " " << symbol.mem.ptr << " (" << typeName << ")";
	}
	else // UDT Array
	{
		ss << symbol.mem.name << " (" << typeName << ")";
	}

	//pProp = new CPropertyGrid( common::str2wstr(ss.str()).c_str() ); 
	pProp = new CPropertyGrid( ss.str() );
	AddProperty(pParentProp, pProp, &symbol,  &STypeData(SymTagArrayType, VT_EMPTY, NULL) );

	return pProp;
}


//------------------------------------------------------------------------
// Pointer Type Preview 
//------------------------------------------------------------------------
CPropertyGrid* visualizer::MakeProperty_PointerData(
	CPropertyGrid *pParentProp, const SSymbolInfo &symbol )
{
	CPropertyGrid *pProp = NULL;

	CComPtr<IDiaSymbol> pPointerType;
	HRESULT hr = symbol.pSym->get_type(&pPointerType);
	ASSERT_RETV(hr == S_OK, pProp);  // PointerType

	CComPtr<IDiaSymbol> pBaseType;
	hr = pPointerType->get_type(&pBaseType);
	ASSERT_RETV(hr == S_OK, pProp);  // BasicDataType or UDTDataType

	void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
	void *newPtr = sharedmemory::MemoryMapping(srcPtr);
	if (!newPtr)
		newPtr = srcPtr; // �����޸𸮿� ���� ����Ÿ�ϰ�� �ּҸ� ����Ѵ�.

	enum SymTagEnum baseSymTag;
	hr = pBaseType->get_symTag((DWORD*)&baseSymTag);

	const string name = dia::GetSymbolName(symbol.pSym); // debug��
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);
	stringstream ss;

	if (SymTagUDT == baseSymTag)
	{
		ss << symbol.mem.name << " 0x" << newPtr;
		ss << (char*)(sharedmemory::CheckValidAddress(newPtr)? " " : " not shared memory");
		ss << " (" << typeName << ")";
	}
	else if (SymTagBaseType == baseSymTag)
	{
		BasicType btype;
		hr = pBaseType->get_baseType((DWORD*)&btype);
		ASSERT_RETV(hr == S_OK, pProp);

		// char* Ÿ���̶�� ��Ʈ���� ����Ѵ�.
		if (btChar == btype)
		{
			ss << symbol.mem.name << " 0x" << newPtr << " {\"";
			ss << (char*)(sharedmemory::CheckValidAddress(newPtr)? newPtr : " not shared memory")  << "\"}";
		}
	}

	if (ss.str().empty()) // default pointer �۾�
	{
		ss << symbol.mem.name << " 0x" << newPtr;
		ss << (char*)(sharedmemory::CheckValidAddress(newPtr)? " " : " not shared memory");
		ss << " (" << typeName << ")";
	}

	//pProp = new CPropertyGrid( common::str2wstr(ss.str()).c_str() );
	pProp = new CPropertyGrid( ss.str() );
	AddProperty( pParentProp, pProp, &symbol, &STypeData(SymTagPointerType, VT_EMPTY, NULL));

	return pProp;
}


//------------------------------------------------------------------------
// UDT type Preview
//------------------------------------------------------------------------
CPropertyGrid* visualizer::MakeProperty_UDTData(
	CPropertyGrid *pParentProp, const SSymbolInfo &symbol )
{
	const string typeName = dia::GetSymbolTypeName(symbol.pSym);

	// �ֻ��� UDT�� �ƴҶ��� Ÿ���� ����Ѵ�.
	stringstream ss;
	ss << symbol.mem.name;
	if (pParentProp)
		ss << "  (" << typeName << ")";

	CPropertyGrid *pProp = 
		//new CPropertyGrid( common::str2wstr(ss.str()).c_str() ); 
		new CPropertyGrid( ss.str() ); 
	AddProperty(pParentProp, pProp, &symbol, &STypeData(SymTagUDT, VT_EMPTY, symbol.mem.ptr));
	
	const bool isVisualizerType = visualizer::MakeVisualizerProperty( n_pProperty, 
		pProp, symbol.mem,  symbol.mem.name);
	
	return (isVisualizerType)? NULL : pProp;
}


//------------------------------------------------------------------------
// pSymbol : Array Type�� ����Ų��. 
//------------------------------------------------------------------------
void visualizer ::MakeProperty_Array(CPropertyGrid *pParentProp, const SSymbolInfo &symbol,
									 const int depth)
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
	basic_bufferstream<char> formatter(valueName, sizeof(valueName));

	if (//SymTagData == elemSymTag ||
		SymTagBaseType == elemSymTag)
	{
		for (ULONGLONG i=0; i < length; i += element_length)
		{
			formatter.seekp(0);
			formatter << "[" << i / element_length << "]" << std::ends;

			void *ptr = (BYTE*)symbol.mem.ptr + i;
			SMemoryInfo arrayElem(valueName, ptr, (size_t)element_length);
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
			SMemoryInfo arrayElem(valueName, ptr, (size_t)element_length);
			SSymbolInfo arraySymbol(pElementType, arrayElem, false);

 			CPropertyGrid *pProp =
 		//		new CPropertyGrid( common::str2wstr(valueName).c_str() );
				new CPropertyGrid( valueName );
			AddProperty( pParentProp, pProp, &arraySymbol, &STypeData(SymTagUDT,VT_EMPTY,NULL));

			MakeProperty_Root(pProp, arraySymbol, depth);
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
CPropertyGrid* visualizer ::MakeProperty_BaseType(
	CPropertyGrid *pParentProp, const std::string valueName, 
	const SSymbolInfo &symbol )
{
	_variant_t value = dia::GetValueFromSymbol(symbol.mem.ptr, symbol.pSym);

	// static ������ ���μ������� ������ �������Ƿ� 0���� ����Ѵ�.
	bool IsNotPrint = false;
	LocationType locType;
	dia::GetSymbolLocation(symbol.pSym, &locType);
	if (LocIsStatic == locType)
	{
		value = 0;
		IsNotPrint  = true;
	}

	CPropertyGrid *pProp = NULL;
	switch (value.vt)
	{
	case VT_I2:
	case VT_I4:
	case VT_R4:
	case VT_R8:
	case VT_BOOL:
	case VT_DECIMAL:
	case VT_UI2:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
		{
			pProp = new CPropertyGrid( valueName );
				//common::str2wstr(valueName).c_str());
			pProp->SetVariantValue( value );

			_variant_t v1 = pProp->GetVariantValue();
			ASSERT(v1.vt == value.vt);
		}
		break;

	case VT_I1:
	case VT_UI1:
		pProp = new CPropertyGrid( valueName ); 
		//	(_variant_t)(int)0, _T("") );
		break;

	default:
		{
		}
		break;
	}
	RETV(!pProp, NULL);

	if (!SetPropertyValue(pProp, value ))
	{
		delete pProp;
		return NULL;
 	}

	if (IsNotPrint)
	{
		// ȭ�鿡 ��µ��� �ʴ� ���� �޸��ּҸ� NULL�� �����ؼ�
		// Refresh ���� �ʰ� �Ѵ�.
		pProp->Enable(false);
 		//pProp->AllowEdit(FALSE);
 		//pProp->Enable(FALSE);
		AddProperty(pParentProp, pProp, &symbol, &STypeData(SymTagData, VT_EMPTY, NULL));
	}
	else
	{
 		AddProperty(pParentProp, pProp, &symbol, &STypeData(SymTagData, value.vt, symbol.mem.ptr));
	}

	return pProp;
}


//------------------------------------------------------------------------
// Property ����Ÿ�� value ���� �����Ѵ�. 
//------------------------------------------------------------------------
bool visualizer ::SetPropertyValue(CPropertyGrid *pProp, _variant_t value)
{
	switch (value.vt)
	{
	case VT_I2:
	case VT_I4:
	case VT_R4:
	case VT_R8:
	case VT_BOOL:
	case VT_DECIMAL:
	case VT_UI2:
	case VT_UI4:
	case VT_INT:
	case VT_UINT:
	case VT_I1:
	case VT_UI1:
		{
			//const size_t optionCnt = pProp->GetOptionCount();
			//if (optionCnt > 0)
			//{
			//	//ASSERT_RETV((int)value < optionCnt, false);
			//	if ((size_t)value < optionCnt)
			//	{
			//		_variant_t var = (_variant_t)pProp->GetOption( (int)value );
			//		pProp->SetValue(var);
			//	}
			//}
			//else
			//{
			//	_variant_t var;
			//	var.ChangeType(pProp->GetValue().vt, &value);
			//	pProp->SetValue(var);
			//}
		}
		break;

		//		{
		//			_variant_t tmp  = (VT_I1 == value.vt)? (int)value.cVal : (int)value.bVal;
		// 			_variant_t var;
		// 			var.ChangeType(pProp->GetValue().vt, &value);
		// 			pProp->SetValue(tmp);
		//  		}
		//  		break;

		// 	case VT_BSTR:
		// 		{
		// 			std::string str;
		// 			operator>>(str);
		// #ifdef _UNICODE
		// 			var.bstrVal = (_bstr_t)common::string2wstring(str).c_str();
		// #else
		// 			var.bstrVal = (_bstr_t)str.c_str();
		// #endif
		// 		}
		// 		break;

	default:
		{
			// Err!!, Property�� value Ÿ���� ���� ���� �� �����ϴ�. 
			ASSERT(0);
			return false;
		}
		break;
	}
	return true;
}


//------------------------------------------------------------------------
// Property�߰�
//------------------------------------------------------------------------
void visualizer ::AddProperty(
								 CPropertyGrid *pParentProp, 
								 CPropertyGrid *prop, 
								 const SSymbolInfo *pSymbol,
								 STypeData *typeData)
{
	RET(!prop);
	RET(!n_pProperty);

	n_pProperty->AddProperty(pParentProp, prop,  pSymbol, typeData);
}
