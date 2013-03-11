//------------------------------------------------------------------------
// Name:    MemoryTree.h
// Author:  jjuiddong
// Date:    3/7/2013
// 
// show object in process memory or shared memory to tree control
//------------------------------------------------------------------------
#pragma once

#include "wx/treectrl.h"

class wxTreeCtrl;
namespace memmonitor
{
	class CMemoryTree : public wxPanel
	{
		DECLARE_EVENT_TABLE()

	public:
		CMemoryTree(wxWindow *parent);
		virtual ~CMemoryTree();

	protected:
		bool LoadMemoryMap();
		bool UpdateMemoryMap();
		void OnTreectrlSelChanged( wxTreeEvent& event );

	protected:
		enum { ID_TREE };
		wxTreeCtrl *m_pTree;

	};
}
