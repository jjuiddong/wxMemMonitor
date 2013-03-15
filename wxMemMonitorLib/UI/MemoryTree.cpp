
#include "stdafx.h"
#include "MemoryTree.h"
#include "wx/treectrl.h"
#include "PropertyWindow.h"
#include "../Control/Global.h"
#include "Frame.h"

using namespace memmonitor;

BEGIN_EVENT_TABLE( memmonitor::CMemoryTree, wxPanel )
	EVT_TREE_SEL_CHANGED(ID_TREE, OnTreectrlSelChanged)
	EVT_CONTEXT_MENU(CMemoryTree::OnContextMenu)
	EVT_MENU(MENU_OPEN_PROPERTY, CMemoryTree::OnMenuOpenProperty)
END_EVENT_TABLE()


CMemoryTree::CMemoryTree(wxWindow *parent) :
	wxPanel(parent, -1, wxDefaultPosition, wxSize(200, 400) )
{
	m_pTree = new wxTreeCtrl(this, ID_TREE, wxDefaultPosition, wxSize(200,400),
		wxTR_HAS_BUTTONS |wxTR_LINES_AT_ROOT|wxTR_ROW_LINES|wxTR_SINGLE |
		wxTR_FULL_ROW_HIGHLIGHT
		);

	UpdateMemoryMap();
}

CMemoryTree::~CMemoryTree()
{

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CMemoryTree::UpdateMemoryMap()
{
	RETV(!m_pTree, false);

	m_pTree->DeleteAllItems();

	wxTreeItemId rootId = m_pTree->AddRoot(wxT("@Root"));
	sharedmemory::MemoryList memList;
	sharedmemory::EnumerateMemoryInfo(memList);
	BOOST_FOREACH(sharedmemory::SMemoryInfo &info, memList)
	{
		wxTreeItemId itemId = m_pTree->AppendItem( rootId, info.name );
		m_pTree->AppendItem( itemId, wxString::Format("size: %d", info.size) );
		m_pTree->AppendItem( itemId, wxString::Format("ptr: 0x%x", (DWORD)info.ptr) );

		//if (selectItemName == info.name)
		//	SelectItem(hItem);
	}
	m_pTree->SortChildren(rootId);
	m_pTree->Expand(rootId);

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryTree::OnTreectrlSelChanged( wxTreeEvent& event )
{
	const wxString text = m_pTree->GetItemText( event.GetItem() );
	PropWindowPtr pPropWnd = GetPropertyWindow();
	RET(!pPropWnd);
	pPropWnd->UpdateSymbol( text );
}


//------------------------------------------------------------------------
// Open Popup Menu
//------------------------------------------------------------------------
void CMemoryTree::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint point = event.GetPosition();
	point = ScreenToClient(point);

	wxMenu menu;
	menu.Append(MENU_OPEN_PROPERTY, wxT("&Open Property"));
	PopupMenu(&menu, point);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CMemoryTree::OnMenuOpenProperty(wxCommandEvent& WXUNUSED(event))
{
	const wxString text = m_pTree->GetItemText(m_pTree->GetSelection());
	GetFrame()->AddPropertyWindow( text );
}
