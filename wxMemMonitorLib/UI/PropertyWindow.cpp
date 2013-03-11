
#include "stdafx.h"
#include "PropertyWindow.h"
#include "PropertyGrid.h"


using namespace memmonitor;

CPropertyWindow::CPropertyWindow(wxWindow *parent)  : 
	wxPropertyGridManager(parent, wxID_ANY, wxDefaultPosition, wxSize(200,150), 
		wxPG_BOLD_MODIFIED |
		wxPG_SPLITTER_AUTO_CENTER |
		wxPG_AUTO_SORT |
		//wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
		//wxPG_TOOLTIPS |
		//wxPG_HIDE_CATEGORIES |
		//wxPG_LIMITED_EDITING |
		wxPG_TOOLBAR |
		wxPG_DESCRIPTION )
{
	Append( new wxPGProperty( _("label2"), _("name1")) );
	Append( new wxPGProperty( _("label1"), _("name3")) );
	Append( new wxPGProperty( _("label3"), _("name4")) );
	const int extraStyle = wxPG_EX_MODE_BUTTONS |
		wxPG_EX_MULTIPLE_SELECTION;
	SetExtraStyle(extraStyle);

	Connect(GetId(), wxEVT_PG_CHANGED,
		wxPropertyGridEventHandler(CPropertyWindow::OnPropertyGridChange) );
}

CPropertyWindow::~CPropertyWindow() 
{

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnPropertyGridChange( wxPropertyGridEvent& event )
{
	int a = 0;

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void	CPropertyWindow::AddProperty(
	CPropertyGrid *pParentProp, 
	CPropertyGrid *prop, 
	const visualizer::SSymbolInfo *pSymbol, 
	STypeData *pTypeData )
{
	if (pParentProp)
	{
		pParentProp->AppendChild(prop);
		//if (!pParentProp->AddSubItem(prop))
		//{
		//	ASSERT(0);
		//}
	}
	else
	{
		Append(prop);
		//if (!CMFCPropertyGridCtrl::AddProperty(prop))
		//{
		//	//			ASSERT(0);
		//}
	}

}
