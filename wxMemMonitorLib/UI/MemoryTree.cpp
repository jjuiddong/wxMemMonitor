
#include "stdafx.h"
#include "MemoryTree.h"

using namespace memmonitor;

CMemoryTree::CMemoryTree(wxWindow *parent) :
	wxTreeCtrl(parent, 0, wxDefaultPosition, wxSize(200, 400), 
		wxTR_HAS_BUTTONS |wxTR_LINES_AT_ROOT|wxTR_ROW_LINES|wxTR_SINGLE )
{

}

CMemoryTree::~CMemoryTree()
{

}


//------------------------------------------------------------------------
// 클래스 초기화
//------------------------------------------------------------------------
bool CMemoryTree::Init()
{
	wxTreeItemId rootId = AddRoot(wxT("Root"));
	AppendItem( rootId, "test1" );
	AppendItem( rootId, "test2" );
	AppendItem( rootId, "test3" );
	AppendItem( rootId, "test4" );
	wxTreeItemId parentId = AppendItem( rootId, "test5" );
	AppendItem( parentId, "test6" );
	Expand(rootId);
	Expand(parentId);


	return true;
}
