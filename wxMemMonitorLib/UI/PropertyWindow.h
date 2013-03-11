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
	class CPropertyGrid;
	class CPropertyWindow : public wxPropertyGridManager
	{
	public:
		CPropertyWindow(wxWindow *parent);
		virtual ~CPropertyWindow();

		void OnPropertyGridChange( wxPropertyGridEvent& event );
		void	AddProperty(CPropertyGrid *pParentProp, CPropertyGrid *prop, 
			const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData);

	};
}
