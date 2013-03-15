//------------------------------------------------------------------------
// Name:    MemoryTree.h
// Author:  jjuiddong
// Date:    3/7/2013
// 
// show memory data
//------------------------------------------------------------------------
#pragma once
#include "wx/treectrl.h"

class wxTreeCtrl;
namespace memmonitor
{
	class CMemoryTree : public wxPanel
	{
		enum  {
			MENU_OPEN_PROPERTY,
		};

	public:
		CMemoryTree(wxWindow *parent);
		virtual ~CMemoryTree();

	protected:
		bool LoadMemoryMap();
		bool UpdateMemoryMap();

		// Event Handler
		DECLARE_EVENT_TABLE()
		void OnTreectrlSelChanged( wxTreeEvent& event );
		void OnContextMenu(wxContextMenuEvent& event);
		void OnMenuOpenProperty(wxCommandEvent& event);

	protected:
		enum { ID_TREE };
		wxTreeCtrl *m_pTree;
	};
}
