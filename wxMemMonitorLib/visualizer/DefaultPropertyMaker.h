//------------------------------------------------------------------------
// Name:    DefaultPropertyMaker.h
// Author:  jjuiddong
// Date:    1/30/2013
// 
// 
//------------------------------------------------------------------------
#ifndef __DEFAULTPROPERTYMAKER_H__
#define __DEFAULTPROPERTYMAKER_H__

#include "VisualizerGlobal.h"

namespace memmonitor 
{
	class CPropertyWindow; 
	class CPropertyItemAdapter;
}

class wxPGProperty;
namespace visualizer
{
	using namespace memmonitor;

	bool		MakeProperty_DefaultForm( CPropertyWindow *pProperties,  const std::string &symbolName );

	bool		MakeProperty_DefaultForm( CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol );

	bool		MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol );

}

#endif // __DEFAULTPROPERTYMAKER_H__
