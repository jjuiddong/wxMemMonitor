//------------------------------------------------------------------------
// Name:    PropertyWindow.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// 
//------------------------------------------------------------------------
#pragma once
#include <wx/propgrid/manager.h>

namespace visualizer
{
	struct SSymbolInfo;
}

namespace memmonitor
{
	class CPropertyItem;
	class CPropertyWindow : public wxPropertyGridManager
	{
	public:
		CPropertyWindow(wxWindow *parent);
		virtual ~CPropertyWindow();

		void UpdateSymbol( const wxString &symbolName );

		void OnPropertyGridChange( wxPropertyGridEvent& event );
		void	AddProperty(CPropertyItem *pParentProp, CPropertyItem *prop, 
			const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData);

	protected:
		wxString	m_CurrentSymbolName;

	};
}
