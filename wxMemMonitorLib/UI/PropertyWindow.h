//------------------------------------------------------------------------
// Name:    PropertyWindow.h
// Author:  jjuiddong
// Date:    3/10/2013
// 
// show memory data
//------------------------------------------------------------------------
#pragma once
#include <wx/propgrid/manager.h>
#include "../Control/Global.h"

namespace visualizer {
	struct SSymbolInfo;
}

namespace memmonitor
{
	class CPropertyWindow : public wxPropertyGridManager
	{
		enum  {
			MENU_OPEN_PROPERTY=500,
			ID_REFRESH_TIMER = 100,
			REFRESH_INTERVAL = 1000,
		};

		struct SPropItem
		{
			std::string typeName;		// dia symbol type name
			std::string symbolTypeName;		// dia symbol type name
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
		void CheckSymbol( const wxString &symbolName );
		bool	FindSymbolUpward( wxPGProperty *pProp, OUT visualizer::SSymbolInfo *pOut );
		void	RefreshPropertyItem( wxPGProperty *pProp );
		void ClearPropItem();

		// Event Handler
		DECLARE_EVENT_TABLE()
		void OnSize(wxSizeEvent& event);
		void OnPropertyGridChange( wxPropertyGridEvent& event );
		void OnPropertyGridSelect( wxPropertyGridEvent& event );
		void OnContextMenu(wxContextMenuEvent& event);
		void OnMenuOpenProperty(wxCommandEvent& event);
		void OnRefreshTimer(wxTimerEvent& event);
		void OnKeyDown(wxKeyEvent& event);

	protected:
		wxString	m_CurrentSymbolName;
		wxTimer	m_Timer;
		PropList	m_PropList;

	};
}
