//------------------------------------------------------------------------
// Name:    PropertyWindow.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// 
//------------------------------------------------------------------------
#pragma once
#include <wx/propgrid/manager.h>
#include "../Control/Global.h"

namespace visualizer
{
	struct SSymbolInfo;
}

namespace memmonitor
{
	class CPropertyWindow : public wxPropertyGridManager
	{
		struct SPropItem
		{
			//wxPGProperty *prop;
			std::string typeName;		// dia symbol type name
			STypeData typeData;
		};
		typedef std::list<SPropItem*> PropList;

	public:
		CPropertyWindow(wxWindow *parent);
		virtual ~CPropertyWindow();

		void UpdateSymbol( const wxString &symbolName );

		void	AddProperty(wxPGProperty *pParentProp, wxPGProperty *prop, 
			const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData);

	protected:
		void ClearPropItem();

		// Event Handler
		void OnPropertyGridChange( wxPropertyGridEvent& event );

	protected:
		wxString	m_CurrentSymbolName;
		PropList	m_PropList;

	};
}
