//------------------------------------------------------------------------
// Name:    PropertyItemAdaptor.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// using wxWidgets Property library
// property class implements
//------------------------------------------------------------------------
#pragma once

#include "../visualizer/VisualizerGlobal.h"
#include <wx/propgrid/property.h>

class wxPGProperty;
namespace memmonitor
{
	class CPropertyItemAdaptor
	{
	public:
		enum PROPERTY_TYPE {
			PROPERTY_STRING,
			PROPTYPE_CATEGORY,
			PROPTYPE_ENUM,
		};

		CPropertyItemAdaptor();
		CPropertyItemAdaptor( std::string label,  PROPERTY_TYPE type = PROPERTY_STRING );
		CPropertyItemAdaptor( wxPGProperty *pProperty );
		virtual ~CPropertyItemAdaptor();

		bool Init( const std::string &valueName, 
			const visualizer::SSymbolInfo &symbol, _variant_t value );

		void SetValue(const wxVariant &var);
		void SetExpanded(bool expand);
		void SetModifiedStatus(bool modify);
		void Enable(bool enable);
		void AddChoice(const std::string &name, const int value=wxPG_INVALID_VALUE );

		wxPGProperty* GetProperty();

	protected:
		wxPGProperty *m_pProperty;

	};

	inline wxPGProperty* CPropertyItemAdaptor::GetProperty() { return m_pProperty; }

}
