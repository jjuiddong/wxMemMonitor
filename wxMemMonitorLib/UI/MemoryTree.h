//------------------------------------------------------------------------
// Name:    MemoryTree.h
// Author:  jjuiddong
// Date:    3/7/2013
// 
// show object in process memory or shared memory to tree control
//------------------------------------------------------------------------
#pragma once

#include "wx/treectrl.h"

namespace memmonitor
{
	class CMemoryTree : public wxTreeCtrl
	{
	public:
		CMemoryTree(wxWindow *parent);
		virtual ~CMemoryTree();
		bool Init();

	protected:

	};
}
