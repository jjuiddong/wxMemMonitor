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
	class CPropertyGrid;
}

namespace visualizer
{
	using namespace memmonitor;

	bool		MakeProperty_DefaultForm( CPropertyWindow *pProperties,  const std::string &symbolName );

	bool		MakeProperty_DefaultForm( CPropertyWindow *pProperties, CPropertyGrid *pParentProp,  
		const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CPropertyWindow *pProperties, CPropertyGrid *pParentProp,  
		const SSymbolInfo &symbol );

	bool		MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, CPropertyGrid *pParentProp,  
		const SSymbolInfo &symbol );

	bool		SetPropertyValue(CPropertyGrid *pProp, _variant_t value);

}

#endif // __DEFAULTPROPERTYMAKER_H__
