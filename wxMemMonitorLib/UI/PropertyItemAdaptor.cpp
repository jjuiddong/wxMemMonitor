
#include "stdafx.h"
#include "PropertyItemAdaptor.h"
#include "../dia/DiaWrapper.h"
#include <wx/propgrid/property.h>
#include <wx/propgrid/advprops.h>

using namespace memmonitor;

CPropertyItemAdaptor::CPropertyItemAdaptor() :
	m_pProperty(NULL)
{
}

CPropertyItemAdaptor::CPropertyItemAdaptor( std::string label,  PROPERTY_TYPE type ) :
	m_pProperty(NULL)
{
	switch (type)
	{
	case PROPERTY_STRING:
		m_pProperty = new wxStringProperty(label, wxPG_LABEL, "<composed>" );	
		break;
	case PROPTYPE_CATEGORY:
		m_pProperty = new wxPropertyCategory(label, wxPG_LABEL );	
		break;
	case PROPTYPE_ENUM:
		m_pProperty = new wxEnumProperty(label, wxPG_LABEL );	
		break;
	}
}

CPropertyItemAdaptor::CPropertyItemAdaptor(wxPGProperty *pProperty) :
	m_pProperty(pProperty)
{

}

CPropertyItemAdaptor::~CPropertyItemAdaptor()
{
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CPropertyItemAdaptor::Init( const std::string &valueName, 
	const visualizer::SSymbolInfo &symbol, _variant_t value )
{
	// static 변수는 프로세스끼리 공유할 수없으므로 0으로 출력한다.
	bool IsNotPrint = false;
	LocationType locType;
	dia::GetSymbolLocation(symbol.pSym, &locType);
	if (LocIsStatic == locType)
	{
		value = 0;
		IsNotPrint  = true;
	}

	wxPGProperty *pProp = NULL;
	switch (value.vt)
	{
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_INT:
		pProp = new wxIntProperty( valueName, wxPG_LABEL, value);
		break;

	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_UINT:
		pProp = new wxUIntProperty( valueName, wxPG_LABEL, value);
		break;

	case VT_R4:
	case VT_R8:
		pProp = new wxFloatProperty( valueName, wxPG_LABEL, value );
		break;

	case VT_BOOL:
		pProp = new wxBoolProperty( valueName, wxPG_LABEL, value );
		break;

	case VT_DECIMAL:
		{
			//pProp = new CPropertyItem( valueName );
			////common::str2wstr(valueName).c_str());
			//pProp->SetVariantValue( value );

			//_variant_t v1 = pProp->GetVariantValue();
			//ASSERT(v1.vt == value.vt);
		}
		break;

		//pProp = new CPropertyItem( valueName ); 
		//	(_variant_t)(int)0, _T("") );
		//pProp->SetVariantValue( value ); // TEST CODE
		break;

	default:
		{
		}
		break;
	}
	//RETV(!pProp, NULL);

	//if (!SetPropertyValue(pProp, value ))
	//{
	//	delete pProp;
	//	return NULL;
	//}
	RETV(!pProp, false);

	if (IsNotPrint)
	{
		pProp->Enable(false);
		pProp->SetModifiedStatus(false);
	}


	m_pProperty = pProp;
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyItemAdaptor::SetExpanded(bool expand)
{
	RET(!m_pProperty);
	m_pProperty->SetExpanded(expand);
}
void CPropertyItemAdaptor::SetModifiedStatus(bool modify)
{
	RET(!m_pProperty);
	m_pProperty->SetModifiedStatus(modify);
}
void CPropertyItemAdaptor::Enable(bool enable)
{
	RET(!m_pProperty);
	m_pProperty->Enable(enable);
}
void CPropertyItemAdaptor::AddChoice(const std::string &name, const int value ) // value = wxPG_INVALID_VALUE
{
	RET(!m_pProperty);
	m_pProperty->AddChoice(name, value);
}
void CPropertyItemAdaptor::SetValue(const wxVariant &var)
{
	RET(!m_pProperty);
	m_pProperty->SetValue(var);
}
