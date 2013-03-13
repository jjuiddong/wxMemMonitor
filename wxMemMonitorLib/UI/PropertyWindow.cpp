
#include "stdafx.h"
#include "PropertyWindow.h"
#include "PropertyItemAdaptor.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/DefaultPropertyMaker.h"
#include "../visualizer/PropertyMaker.h"


using namespace memmonitor;

CPropertyWindow::CPropertyWindow(wxWindow *parent)  : 
	wxPropertyGridManager(parent, wxID_ANY, wxDefaultPosition, wxSize(500,150), 
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
	const int extraStyle = wxPG_EX_MODE_BUTTONS |
		wxPG_EX_MULTIPLE_SELECTION;
	SetExtraStyle(extraStyle);

	Connect(GetId(), wxEVT_PG_CHANGED,
		wxPropertyGridEventHandler(CPropertyWindow::OnPropertyGridChange) );
}

CPropertyWindow::~CPropertyWindow() 
{
	ClearPropItem();

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::UpdateSymbol( const wxString &symbolName )
{
	if (m_CurrentSymbolName == symbolName)
		return;

	wxPropertyGrid *pPropGrid = GetGrid();
	pPropGrid->Clear();
	ClearPropItem();

	std::string tmpStr = symbolName;
	std::string str = sharedmemory::ParseObjectName(tmpStr);
	const bool result = visualizer::MakeProperty_DefaultForm(this, tmpStr);

	wxPGVIterator it;
	for ( it = pPropGrid->GetVIterator( wxPG_ITERATE_FIXED_CHILDREN ); !it.AtEnd(); it.Next() )
		it.GetProperty()->SetExpanded( true );

	//CComPtr<IDiaSymbol> pSymbol = CDiaWrapper::Get()->FindType(str);
	//if (!pSymbol)
	//{
	//	::AfxMessageBox(
	//		common::formatw("\"%s\" 해당하는 심볼은 Pdb파일에 없습니다.", 
	//		tmpStr.c_str()).c_str() );
	//	SetWindowTextW(L"Not Found");
	//	return;
	//}

	//sharedmemory::SMemoryInfo memInfo;
	//if (!sharedmemory::FindMemoryInfo(tmpStr, memInfo))
	//{
	//	::AfxMessageBox( 
	//		common::formatw("공유메모리에 %s 타입의 정보가 없습니다.",
	//		tmpStr.c_str()).c_str() );
	//	SetWindowTextW(L"Not Found");
	//	return;
	//}

	//// Output창에 출력
	//global::PrintOutputWnd( "[" + tmpStr + " ] 심볼 변경" );
	//dbg::Print(  "%s 심볼 변경", tmpStr.c_str() );
	////

	//SetWindowTextW(symbolName);
	//m_CurrentSymbolName = symbolName;
	//ChangeComboBoxFocus(symbolName);

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void	CPropertyWindow::AddProperty( wxPGProperty *pParentProp, wxPGProperty *prop, 
	const visualizer::SSymbolInfo *pSymbol, STypeData *pTypeData )
{
	SPropItem *p = new SPropItem;
	//p->prop = prop;
	p->typeData = *pTypeData;
	if (pSymbol)
	{
		p->typeName = dia::GetSymbolName(pSymbol->pSym);
		p->typeData.ptr = pSymbol->mem.ptr;
	}
	m_PropList.push_back(p);

	prop->SetClientData(p);

	if (pParentProp)
	{
		AppendIn( pParentProp, prop );
	}
	else
	{
		wxPGProperty *pg = Append(prop);
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnPropertyGridChange( wxPropertyGridEvent& event )
{
	wxPGProperty* property = event.GetProperty();
	const wxString& name = property->GetName();
	const wxString& label = property->GetLabel();
	wxAny value = property->GetValue();
	if ( value.IsNull() )
		return;

	SPropItem *pItem = (SPropItem*)property->GetClientData();
	if (pItem && pItem->typeData.ptr)
	{
		const wxVariant curVar = property->GetValue();
		_variant_t var = wxVariant2Variant(pItem->typeData.vt, curVar);

		// 메모리에 저장
		dia::SetValue( pItem->typeData.ptr, var);

		//_variant_t curVar = pProp->GetValue();
		//_variant_t var;
		//if (pProp->GetOptionCount() > 0)
		//{
		//	CString str = (LPCTSTR) (_bstr_t)curVar;
		//	const int idx = FindOption(pProp, str);
		//	ASSERT_RET( idx >= 0);
		//	_variant_t tmp = idx;
		//	var.ChangeType(VT_INT, &tmp);
		//}
		//else
		//{
		//	var.ChangeType(pItem->typeData.vt, &curVar);
		//}

	}
}


//------------------------------------------------------------------------
// remove m_PropList
//------------------------------------------------------------------------
void CPropertyWindow::ClearPropItem()
{
	BOOST_FOREACH(auto &item, m_PropList)
	{
		SAFE_DELETE(item);
	}
	m_PropList.clear();
}
