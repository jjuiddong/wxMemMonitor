/**
Name:   DefaultPropertyMaker.h
Author:  jjuiddong
Date:    1/30/2013

*/
#pragma once

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

	bool		MakeProperty_DefaultForm( CPropertyWindow *pProperties,  wxPGProperty *pParentProp,
		const std::string &symbolName );

	bool		MakeProperty_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, bool IsApplyVisualizer=true );

	bool		MakePropertyChild_DefaultForm(  CPropertyWindow *pProperties, wxPGProperty *pParentProp,  
		const SSymbolInfo &symbol, bool IsApplyVisualizer=true );
}
