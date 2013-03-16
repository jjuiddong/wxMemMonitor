
#include "stdafx.h"
#include "PropertyWindow.h"
#include "PropertyItemAdapter.h"
#include "LogWindow.h"
#include "../dia/DiaWrapper.h"
#include "../visualizer/DefaultPropertyMaker.h"
#include "../visualizer/PropertyMaker.h"


using namespace memmonitor;
using namespace dia;
using namespace visualizer;

BEGIN_EVENT_TABLE( memmonitor::CPropertyWindow, wxPropertyGridManager )
	EVT_CONTEXT_MENU(CPropertyWindow::OnContextMenu)
	EVT_MENU(MENU_OPEN_PROPERTY, CPropertyWindow::OnMenuOpenProperty)
	EVT_TIMER(ID_REFRESH_TIMER, CPropertyWindow::OnRefreshTimer)
END_EVENT_TABLE()


CPropertyWindow::CPropertyWindow(wxWindow *parent)  : 
	wxPropertyGridManager(parent, wxID_ANY, wxDefaultPosition, wxSize(500,300), 
		wxPG_BOLD_MODIFIED |
		wxPG_SPLITTER_AUTO_CENTER |
		wxPG_AUTO_SORT |
		//wxPG_HIDE_MARGIN|wxPG_STATIC_SPLITTER |
		//wxPG_TOOLTIPS |
		//wxPG_HIDE_CATEGORIES |
		//wxPG_LIMITED_EDITING |
		wxPG_TOOLBAR
		//wxPG_DESCRIPTION 
		)
{
	const int extraStyle = wxPG_EX_MODE_BUTTONS |
		wxPG_EX_MULTIPLE_SELECTION;
	SetExtraStyle(extraStyle);

	m_Timer.SetOwner(this, ID_REFRESH_TIMER);
	m_Timer.Start( REFRESH_INTERVAL );

	Connect(GetId(), wxEVT_PG_CHANGED,
		wxPropertyGridEventHandler(CPropertyWindow::OnPropertyGridChange) );
	Connect(GetId(), wxEVT_PG_SELECTED,
		wxPropertyGridEventHandler(CPropertyWindow::OnPropertyGridSelect) );	
}

CPropertyWindow::~CPropertyWindow() 
{
	m_Timer.Stop();
	ClearPropItem();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::UpdateSymbol( const wxString &symbolName )
{
	if (m_CurrentSymbolName == symbolName)
		return;
	if (symbolName == "@Root")
		return;

	GetLogWindow()->PrintText( "UpdateSymbol = " +  symbolName  + "\n" );

	wxPropertyGrid *pPropGrid = GetGrid();
	pPropGrid->Clear();
	ClearPropItem();

	std::string tmpStr = symbolName;
	std::string str = sharedmemory::ParseObjectName(tmpStr);
	const bool result = visualizer::MakeProperty_DefaultForm(this, tmpStr);

	// root node expand
	wxPGVIterator it;
	for ( it = pPropGrid->GetVIterator( wxPG_ITERATE_FIXED_CHILDREN ); !it.AtEnd(); it.Next() )
	{
		it.GetProperty()->SetExpanded( true );
		break;
	}

	CheckSymbol( symbolName );

	//SetWindowTextW(symbolName);
	m_CurrentSymbolName = symbolName;
	//ChangeComboBoxFocus(symbolName);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::CheckSymbol( const wxString &symbolName )
{
	std::string tmpStr = symbolName;
	std::string str = sharedmemory::ParseObjectName(tmpStr);

	CComPtr<IDiaSymbol> pSymbol = CDiaWrapper::Get()->FindType(str);
	if (!pSymbol)
	{
		GetLogWindow()->PrintText( 
			"\"" + tmpStr + "\"" +
			" 해당하는 심볼은 Pdb파일에 없습니다.\n" );
		return;
	}

	sharedmemory::SMemoryInfo memInfo;
	if (!sharedmemory::FindMemoryInfo(tmpStr, memInfo))
	{
		GetLogWindow()->PrintText( 
			"공유메모리에 " + tmpStr + " 타입의 정보가 없습니다.\n" );
		return;
	}

	// Output창에 출력
	//dbg::Print(  "%s 심볼 변경", tmpStr.c_str() );
	//
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
// Property Change Event Handler
//------------------------------------------------------------------------
void CPropertyWindow::OnPropertyGridChange( wxPropertyGridEvent& event )
{
	wxPGProperty* pProp = event.GetProperty();
	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	if (pItemData && pItemData->typeData.ptr)
	{
		const wxVariant curVar = pProp->GetValue();
		_variant_t var = wxVariant2Variant(pItemData->typeData.vt, curVar);
		// write memory
		dia::SetValue( pItemData->typeData.ptr, var);
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
	wxPGProperty* pProp = event.GetProperty();
	RET(!pProp);
	wxString name = pProp->GetName();

	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	if (pItemData && pItemData->typeData.vt == VT_EMPTY &&
		pProp->GetChildCount() <= 0)
	{
		SSymbolInfo symbol;
		if (!FindSymbolUpward( pProp, &symbol ))
			return;
		if (visualizer::MakePropertyChild_DefaultForm( this, pProp, symbol))
		{
			//pProp->Expand();
			//AdjustLayout();
		}
	}
}


//------------------------------------------------------------------------
// find symbol from child to parent node
//------------------------------------------------------------------------
bool	CPropertyWindow::FindSymbolUpward( wxPGProperty *pProp, OUT SSymbolInfo *pOut )
{
	using std::string;

	RETV(!pProp, false);
	RETV(!pOut, false);

	SPropItem *pItemData = (SPropItem*)pProp->GetClientData();
	RETV(!pItemData, false);

	string name =  pItemData->typeName;
	const int idx = name.find(' ');
	string searchName;
	if (string::npos == idx)
		searchName = name;
	else
		searchName = name.substr(0, idx);

	wxPGProperty *pParentProp = pProp->GetParent();
	if (pParentProp)
	{
		bool retry = false;
		SSymbolInfo symbol;
		if (!FindSymbolUpward( pParentProp, &symbol ))
		{
			retry = true;
		}
		else
		{
			if (!visualizer::Find_ChildSymbol(searchName, symbol, pOut))
				retry = true;
		}

		if (retry)
		{
			// 찾기를 실패했다면, 현재 노드에서 찾기를 시도한다.
			const string typeName = sharedmemory::ParseObjectName(searchName);
			pOut->pSym = CDiaWrapper::Get()->FindType( typeName );
			RETV(!pOut->pSym, false);
			pOut->mem = SMemoryInfo(pItemData->typeName.c_str(), pItemData->typeData.ptr, 0);
		}
	}
	else
	{
		const string typeName = sharedmemory::ParseObjectName(searchName);
		pOut->pSym = CDiaWrapper::Get()->FindType( typeName );
		RETV(!pOut->pSym, false);
		pOut->mem = SMemoryInfo(pItemData->typeName.c_str(), pItemData->typeData.ptr, 0); 
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void	CPropertyWindow::RefreshPropertyItem( wxPGProperty *pProp )
{
	// if edit property item then return
	if (GetGrid()->IsEditorFocused() && GetSelectedProperty() == pProp)
		return;

	SPropItem *pItem = (SPropItem*)pProp->GetClientData();
	if (pItem && 
		(SymTagData == pItem->typeData.symtag 
		|| SymTagBaseType == pItem->typeData.symtag 
		||  SymTagEnum == pItem->typeData.symtag 
		)
		&& pItem->typeData.ptr
		&& pItem->typeData.vt != VT_EMPTY )
	{
		_variant_t value = dia::GetValue(pItem->typeData.ptr, pItem->typeData.vt);
		CPropertyItemAdapter adaptor(pProp);
		adaptor.SetVariant( value );
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnContextMenu(wxContextMenuEvent& event)
{
	wxPoint point = event.GetPosition();
	point = ScreenToClient(point);
	wxMenu menu;
	menu.Append(MENU_OPEN_PROPERTY, wxT("&Open Property"));
	PopupMenu(&menu, point);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CPropertyWindow::OnMenuOpenProperty(wxCommandEvent& event)
{

}


//------------------------------------------------------------------------
// Property Refresh Timer
//------------------------------------------------------------------------
void CPropertyWindow::OnRefreshTimer(wxTimerEvent& event)
{
	wxPropertyGrid *pPropGrid = GetGrid();
	wxPGVIterator it;
	for ( it = pPropGrid->GetVIterator( wxPG_ITERATE_FIXED_CHILDREN ); !it.AtEnd(); it.Next() )
		RefreshPropertyItem( it.GetProperty() );
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
