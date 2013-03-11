//------------------------------------------------------------------------
// Name:    VisualizerPropertyMaker.h
// Author:  jjuiddong
// Date:    1/29/2013
// 
// 
//------------------------------------------------------------------------
#ifndef __VISUALIZERPROPERTYMAKER_H__
#define __VISUALIZERPROPERTYMAKER_H__

#include "VisualizerDefine.h"
#include "VisualizerGlobal.h"

namespace memmonitor 
{ 
	class CPropertyWindow; 
	class CPropertyItem;
}

struct IDiaSymbol;
namespace visualizer
{
	using namespace memmonitor;

	 // init
	bool OpenVisualizerScript( const std::string &fileName );
	void Release();

	// make property
	bool	MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
		CPropertyItem *pParentProp, 
		const sharedmemory::SMemoryInfo &memInfo, const std::string &symbolName );

	bool	MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
		CPropertyItem *pParentProp, const SSymbolInfo &symbol );

	// find
	bool Find_ChildSymbol(  const std::string findSymbolName, 
		IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut );

}

#endif //  __VISUALIZERPROPERTYMAKER_H__
