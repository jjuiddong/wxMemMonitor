//------------------------------------------------------------------------
// Name:    VisualizerPropertyMaker.h
// Author:  jjuiddong
// Date:    1/29/2013
// 
// make property attribute with visualizer autoexp.txt script
//------------------------------------------------------------------------
#ifndef __VISUALIZERPROPERTYMAKER_H__
#define __VISUALIZERPROPERTYMAKER_H__

#include "VisualizerDefine.h"
#include "VisualizerGlobal.h"

namespace memmonitor { 
	class CPropertyWindow; 
	class CPropertyItemAdapter;
}

struct IDiaSymbol;
class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;

	 // init
	bool OpenVisualizerScript( const std::string &fileName );
	void Release();

	// make property
	bool	MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
		wxPGProperty *pParentProp, 
		const sharedmemory::SMemoryInfo &memInfo, const std::string &symbolName );

	bool	MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
		wxPGProperty *pParentProp, const SSymbolInfo &symbol );

	// find
	bool Find_ChildSymbol(  const std::string findSymbolName, 
		IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut );

}

#endif //  __VISUALIZERPROPERTYMAKER_H__
