//------------------------------------------------------------------------
// Name:    PropertyGrid.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// using wxWidgets Property library
// property class implements
//------------------------------------------------------------------------
#pragma once

#include <wx/propgrid/property.h>

namespace memmonitor
{
	class CPropertyGrid : public wxPGProperty
	{
	public:
		CPropertyGrid( std::string label );
		virtual ~CPropertyGrid();

		void SetVariantValue( const _variant_t &var );
		_variant_t GetVariantValue();

	protected:

	};

	wxVariant variant2wxVariant(const _variant_t &var);
	_variant_t wxVariant2variant(const wxVariant &wxVar);

}
