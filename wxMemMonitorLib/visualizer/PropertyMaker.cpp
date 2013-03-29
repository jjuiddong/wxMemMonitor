
#include "stdafx.h"
#include "PropertyMaker.h"
#include "VisualizerParser.h"
#include "../Dia/DiaWrapper.h"
#include "DefaultPropertyMaker.h"
#include <atlcomcli.h>
#include "../Control/Global.h"
#include "../ui/LogWindow.h"

namespace visualizer
{
	using namespace parser;

	SVisualizerScript* FindVisualizer( const std::string &typeName );

	// search visualizer
	SVisualizerScript* SearchVisualizerScript(SType_Stmt *psymT);
	bool CompareTypes( SType_Stmt *psymT, SType_Stmts *pvisT);
	bool CompareType( SType_Stmt *psymT, SType_Stmt *pvisT,OUT bool &isAstrisk );
	bool CompareTemplateTypes( SType_TemplateArgs *psymT, 
		SType_TemplateArgs *pvisT );
	SType_Stmt* ParseType( INOUT std::string &typeName );
	SType_TemplateArgs* ParseTemplateType( INOUT std::string &typeName );


	// parse visualizer
	void MakeProperty_Visualizer( SVisualizer *pvis, const SSymbolInfo &symbol  );

	void MakeProperty_AutoExpand( SAutoExp *pautoexp, const SSymbolInfo &symbol );

	void MakePropertyStatements( SStatements *pstmt, const SSymbolInfo &symbol, const std::string &title="" );

	void MakePropertySimpleExpression( SSimpleExp *pexp, const SSymbolInfo &symbol );

	void MakePropertyExpression( SExpression *pexp, const SSymbolInfo &symbol, const std::string &title="" );

	void MakePropertyIfStmt( SIf_Stmt *pif_stmt, const SSymbolInfo &symbol );

	void MakePropertyIteratorStmt( SVisBracketIterator_Stmt *pItor_stmt, 
		const SSymbolInfo &symbol );

	bool MakePropertyElifStmt( SElif_Stmt *pelif_stmt, const SSymbolInfo &symbol );


	// evaluate
	_variant_t Eval_Expression( SExpression *pexp, const SSymbolInfo &symbol);

	_variant_t Eval_Variable( SExpression *pexp, const SSymbolInfo &symbol );

	// Find
	bool Find_Variable( SExpression *pexp, 
		IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut );

	bool Find_Variable_FromId( const std::string &varId, 
		IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut );

	void Disp_Expression( SExpression *pexp );
	char GetAsciiFromTokentype(Tokentype tok);
	void CheckError( bool condition, const std::string &msg="error" );

}

using namespace dia;
using namespace std;
using namespace visualizer;

visualizer::parser::SVisualizerScript *m_pVisScr = NULL;
std::string m_SymbolName;
SMemInfo m_MemInfo;
CPropertyWindow *m_pPropertiesCtrl= NULL;
wxPGProperty *m_pParentProperty = NULL;

// exception class
class VisualizerScriptError
{
public:
	VisualizerScriptError(const string &msg) : m_Msg(msg) {}
	string m_Msg;
};


/**
 @brief ���� ������ ���� ptr�� ���� ����Ű�� ������ ���� ������� �Լ���.
 */
DWORD Point2PointValue(DWORD ptr)
{
	DWORD value = 0;
	if (ptr)
		value = *(DWORD*)(void*)ptr;
	return value;
}


//------------------------------------------------------------------------
// Visualizer ��ũ��Ʈ�� �д´�.
//------------------------------------------------------------------------
bool visualizer::OpenVisualizerScript( const std::string &fileName )
{
	RemoveVisualizerScript(m_pVisScr);
	CParser parser;
	m_pVisScr = parser.Parse(fileName);
	if (!m_pVisScr)
		return false;
	return true;
}

void visualizer::Release()
{
	RemoveVisualizerScript(m_pVisScr);

}


//------------------------------------------------------------------------
// Property ����
// symbolName : �����޸𸮿� ����� �ɺ��̸�
//------------------------------------------------------------------------
bool	visualizer::MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
											   wxPGProperty *pParentProp, 
											   const SMemInfo &memInfo, const string &symbolName )
{
	const std::string str = ParseObjectName(symbolName);
	SVisualizerScript *pVisScript = FindVisualizer(str);
	if (pVisScript)
	{
		IDiaSymbol *pSymbol = dia::FindType(str);
		RETV(!pSymbol, false);

		m_SymbolName = str;
		m_MemInfo = memInfo;
		m_pPropertiesCtrl = pPropertiesWnd;
		m_pParentProperty = pParentProp;

		try
		{
			if (VisualizerScript_Visualizer == pVisScript->kind)
				MakeProperty_Visualizer(pVisScript->vis, 
					SSymbolInfo(pSymbol, SMemInfo(symbolName.c_str(), memInfo.ptr,0)));
			else 
				MakeProperty_AutoExpand(pVisScript->autoexp, 
					SSymbolInfo(pSymbol, SMemInfo(symbolName.c_str(), memInfo.ptr,0)));
		}
		catch (VisualizerScriptError &e)
		{
			GetLogWindow()->PrintText( e.m_Msg );
		}
		return true;
	}
	else
	{
		return false;
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	visualizer::MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
	wxPGProperty *pParentProp, const SSymbolInfo &symbol )
{
	// Ÿ�Խɺ��� ��´�.
	dia::SymbolState symState;
	IDiaSymbol *pBaseType = dia::GetBaseTypeSymbol(symbol.pSym, symState);
	RETV(!pBaseType, false);

	string typeName = dia::GetSymbolName(pBaseType);
	const bool result = MakeVisualizerProperty(pPropertiesWnd, pParentProp, 
		symbol.mem, typeName);

	if (dia::NEW_SYMBOL == symState)
		pBaseType->Release();
	return result;
}


//------------------------------------------------------------------------
// visualizer ������Ƽ ����
//------------------------------------------------------------------------
void visualizer::MakeProperty_Visualizer( SVisualizer *pvis, const SSymbolInfo &symbol )
{
	RET(!pvis);
	// �ϴ� preview�� ��� 
	MakePropertyStatements( pvis->preview, symbol );
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void visualizer::MakeProperty_AutoExpand( SAutoExp *pautoexp, const SSymbolInfo &symbol )
{
	RET(!pautoexp);

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void visualizer::MakePropertyStatements( SStatements *pstmt, const SSymbolInfo &symbol, const std::string &title ) // title
{
	RET(!pstmt);
	SStatements *node = pstmt;
	while (node)
	{
		switch (node->kind)
		{
		case Stmt_Expression: MakePropertyExpression(node->exp, symbol, title); break;
		case Stmt_SimpleExpression: MakePropertySimpleExpression(node->simple_exp, symbol); break;
		case Stmt_If: MakePropertyIfStmt(node->if_stmt, symbol); break;
		case Stmt_Bracket_Iterator: MakePropertyIteratorStmt(node->itor_stmt, symbol); break;
		}	
		node = node->next;
	}
}


//------------------------------------------------------------------------
// Simple_Expression ó��
//------------------------------------------------------------------------
void visualizer::MakePropertySimpleExpression( SSimpleExp *pexp, const SSymbolInfo &symbol )
{
	RET(!pexp);	
	RET(!pexp->text);
	RET(!pexp->expr);

	SSymbolInfo findVar;
	const bool result = Find_Variable(pexp->expr, symbol, &findVar);
	CheckError(result, "Simple Expression Error!!, not found variable" );

	findVar.mem.name = pexp->text->str;
	MakeProperty_DefaultForm(m_pPropertiesCtrl, m_pParentProperty, findVar);	
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void visualizer::MakePropertyIfStmt( SIf_Stmt *pif_stmt, const SSymbolInfo &symbol )
{
	RET(!pif_stmt);
	if (Eval_Expression(pif_stmt->cond, symbol) == CComVariant(true))
	{
		MakePropertyStatements( pif_stmt->stmts, symbol );
	}
	else
	{
		if (MakePropertyElifStmt( pif_stmt->elif_stmt, symbol ))
		{
			// �ƹ��� ����
		}
		else
		{
			MakePropertyStatements( pif_stmt->else_stmts, symbol );			
		}
	}
}


//------------------------------------------------------------------------
// �ݺ� ��ɹ� ó��
//------------------------------------------------------------------------
void visualizer::MakePropertyIteratorStmt( SVisBracketIterator_Stmt *pitor_stmt,
							  const SSymbolInfo &symbol )
{
	RET(!pitor_stmt);
	RET(!pitor_stmt->stmts);

	switch (pitor_stmt->kind)
	{
	case Iter_Array: break;

	case Iter_List:
		{
			CheckError( pitor_stmt->stmts->head && pitor_stmt->stmts->next, "#list head, next not setting" );
			CheckError( pitor_stmt->stmts->expr || pitor_stmt->disp_stmt , "#list expr, disp_stmt not setting" );

			// size ����, IDiaSymbol
			_variant_t size_v;
			if (pitor_stmt->stmts->size)
			{
				size_v = Eval_Variable(pitor_stmt->stmts->size, symbol);
				CheckError( size_v.vt != VT_EMPTY, "#list size expression error" );
			}
			int size = (int)size_v;
			if (!pitor_stmt->stmts->size)
				size = 100; // default

			_variant_t skip;
			if (pitor_stmt->stmts->skip)
			{
				skip = Eval_Variable(pitor_stmt->stmts->skip, symbol);
				CheckError( skip.vt != VT_EMPTY, "#list skip expression error" );
			}
			const DWORD skipPtr = Point2PointValue((DWORD)skip);

			const _variant_t node = Eval_Variable(pitor_stmt->stmts->head, symbol);
			CheckError( node.vt != VT_EMPTY, "#list head expression error" );
			DWORD nodePtr = Point2PointValue((DWORD)node);

			SSymbolInfo each;
			const bool result = Find_Variable(pitor_stmt->stmts->head, symbol, &each);
 			CheckError( result, "#list head expression error, $e not found" );

			int count = 0;
			while (nodePtr 
				&& (!skipPtr || (skipPtr && nodePtr != skipPtr))
				&& (count < size))
			{
				std::stringstream ss;
				ss << "[" << count << "]";
				const std::string title = ss.str();

				MakePropertyExpression( pitor_stmt->stmts->expr, each, title ); /// display
				MakePropertyStatements( pitor_stmt->disp_stmt, each, title ); /// display

				const bool result = Find_Variable(pitor_stmt->stmts->next, each, &each);
				CheckError( result, "#list head expression error, $e not found" );

				nodePtr = Point2PointValue((DWORD)each.mem.ptr);
				++count;
			}
		}
		break;

	case Iter_Tree: break;
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakePropertyElifStmt( SElif_Stmt *pelif_stmt, const SSymbolInfo &symbol  )
{
	RETV(!pelif_stmt, false);
	if (Eval_Expression(pelif_stmt->cond, symbol) == CComVariant(true))
	{
		MakePropertyStatements( pelif_stmt->stmts, symbol );
		return true;
	}
	else
	{
		return MakePropertyElifStmt( pelif_stmt->next, symbol );
	}
}


/**
 @brief 
 */
void visualizer::MakePropertyExpression( SExpression *pexp, const SSymbolInfo &symbol, const std::string &title ) // title = ""
{
	RET(!pexp);

	switch (pexp->kind)
	{
	case CondExprK:
	case AddTermK:
	case MulTermK:
		break;

	case DispFormatK:
		MakePropertyExpression(pexp->rhs, symbol);
		break;

	case VariableK:
		{
			SSymbolInfo findSymbol;
			const bool result = Find_Variable(pexp, symbol, &findSymbol);
			CheckError(result, " variable expression error!!, undetected" );
			if (!title.empty())
				findSymbol.mem.name = title;
			visualizer::MakeProperty_DefaultForm( m_pPropertiesCtrl, m_pParentProperty, findSymbol);
		}
		break;

	case IndirectK:
	case NumberK:
	case StringK:
		break;
	}
}


//------------------------------------------------------------------------
// typeName�� �ش�Ǵ� Visualizer ��ũ��Ʈ ������
//------------------------------------------------------------------------
SVisualizerScript* visualizer::FindVisualizer(const std::string &typeName )
{
	RETV(!m_pVisScr, NULL);

	std::string parseName = typeName;
	SType_Stmt *pTypeStmt = ParseType(parseName);

	SVisualizerScript *pscr = SearchVisualizerScript(pTypeStmt);
	RemoveType_Stmt(pTypeStmt);
	return pscr;
}


//------------------------------------------------------------------------
// Dia ���̺귯������ �Ѱ��� Ÿ�� ��Ʈ���� SType_Stmt
// ���·� �м��ؼ� �����Ѵ�.
//------------------------------------------------------------------------
SType_Stmt* visualizer::ParseType( INOUT string &typeName  )
{
	SType_Stmt *p = new SType_Stmt;

	// ���� ��ū���� index�� ã�´�.
	list<int> indices;
	indices.push_back(typeName.find('<'));
	indices.push_back(typeName.find(','));
	indices.push_back(typeName.find('>'));
	indices.sort();
	int idx = indices.front();
	while (idx == string::npos && !indices.empty())
	{
		if (indices.size() == 1)
			break;
		indices.pop_front();
		idx = indices.front();
	}

	if (string::npos == idx)
	{
		p->id = typeName;
		typeName.clear();
		p->templateArgs = NULL;
	}
	else  if ('<' == typeName[ idx])
	{
		p->id = typeName.substr(0, idx);
		typeName = typeName.substr(idx+1, typeName.size()-1); // next string
		p->templateArgs = ParseTemplateType(typeName);

		ASSERT_RETV(!typeName.empty(), p);
		const int rtIdx = typeName.find('>');
		ASSERT_RETV(rtIdx < 2, p); // ��ĭ ����� '>' �� ������, >> �� �������� 
		typeName = typeName.substr(rtIdx, typeName.size()-1); // next string
	}
	else  
	{
		p->id = typeName.substr(0, idx);
		typeName = typeName.substr(idx, typeName.size()-1); // next string
		p->templateArgs = NULL;
	}
	return p;
}


//------------------------------------------------------------------------
// < typename, typename .. > parsing
//------------------------------------------------------------------------
SType_TemplateArgs* visualizer::ParseTemplateType( INOUT string &typeName )
{
	SType_TemplateArgs *p = new SType_TemplateArgs;
	p->type = ParseType(typeName);
	p->next = NULL;

	if (typeName.empty())
		return p;

	if (typeName[0] == ',')
	{
		typeName = typeName.substr(1, typeName.size()-1); // next string
		p->next = ParseTemplateType(typeName);
	}
	return p;
}


//------------------------------------------------------------------------
// psymT Ÿ�԰� ���� visualizer script�� �����Ѵ�.
//------------------------------------------------------------------------
SVisualizerScript* visualizer::SearchVisualizerScript(SType_Stmt *psymT)
{
	RETV(!m_pVisScr, NULL);

	SVisualizerScript *node = m_pVisScr;
	while (node)
	{
		switch (node->kind)
		{
		case VisualizerScript_AutoExp:
 			if (CompareTypes(psymT, node->autoexp->match_type))
 				return node;
			break;

		case VisualizerScript_Visualizer:
			if (CompareTypes(psymT, node->vis->matchType))
				return node;
			break;
		}
		node = node->next;
	}
	return NULL;
}


//------------------------------------------------------------------------
// visualizer script �� OR �������� �ϳ� �̻��� Ÿ����
// ������ �� �ִ�. ���߿� �� Ÿ���̶� ���ٸ�,
// �ش� visualizer �� ��Ī�ȴ�.
//------------------------------------------------------------------------
bool visualizer::CompareTypes( SType_Stmt *psymT, 
																	  SType_Stmts *pvisT)
{
	SType_Stmts *node = pvisT;
	while (node)
	{
		bool isAsterisk = false;
		if (CompareType(psymT, node->type, isAsterisk))
			return true;
		node = node->next;
	}
	return false;
}


//------------------------------------------------------------------------
// t1, t2 �ΰ��� Ÿ���� ���ؼ� ���ٸ� true�� �����Ѵ�.
// psymT : dia ���� �Ѿ���� Ÿ�Ը�
// pvisT : visualizer ��ũ��Ʈ Ÿ�� '*' �ƽ��׸�ũ�� �����Ѵ�.
// * �� �����ٸ� isAstrick ���� true�� ���ϵȴ�.
//------------------------------------------------------------------------
bool visualizer::CompareType( SType_Stmt *psymT, SType_Stmt *pvisT, OUT bool &isAstrisk )
{
	RETV(!psymT && !pvisT, true);
	RETV(!psymT, false);
	RETV(!pvisT, false);

	isAstrisk  = false;
	if (pvisT->id == "*")
	{
		isAstrisk = true;
		return true;
	}

	if (psymT->id == pvisT->id)
	{
		if (psymT->templateArgs && pvisT->templateArgs)
		{
			return CompareTemplateTypes(psymT->templateArgs, pvisT->templateArgs);
		}
		else if (!psymT->templateArgs && !pvisT->templateArgs)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


//------------------------------------------------------------------------
// ���ø� Ÿ�Կ��� �ΰ��� 
//------------------------------------------------------------------------
bool visualizer::CompareTemplateTypes( SType_TemplateArgs *psymT, 
						  SType_TemplateArgs *pvisT )
{
	RETV(!psymT && !pvisT, true);
	RETV(!psymT, false);
	RETV(!pvisT, false);

	bool isAsterisk = false;
	if (CompareType(psymT->type, pvisT->type, isAsterisk))
	{
		if (isAsterisk)
			return true;
		return CompareTemplateTypes(psymT->next, pvisT->next);
	}
	return false;
}


//------------------------------------------------------------------------
//  SExpression ���
//------------------------------------------------------------------------
void visualizer::Disp_Expression( SExpression *pexp )
{

}


//------------------------------------------------------------------------
// SExpression ���� �����Ѵ�.
//------------------------------------------------------------------------
_variant_t visualizer::Eval_Expression( SExpression *pexp, const SSymbolInfo &symbol )
{
	_variant_t reval;
	RETV(!pexp, reval);

	switch (pexp->kind)
	{
	case CondExprK:
		{
			switch (pexp->op)
			{
			case LT: reval = ((float)Eval_Expression(pexp->lhs, symbol) < (float)Eval_Expression(pexp->rhs, symbol)); break;
			case RT: reval = ((float)Eval_Expression(pexp->lhs, symbol) > (float)Eval_Expression(pexp->rhs, symbol)); break;
			case LTEQ: /* <= */
				reval = ((float)Eval_Expression(pexp->lhs, symbol) < (float)Eval_Expression(pexp->rhs, symbol)) 
					|| ((float)Eval_Expression(pexp->lhs, symbol) == (float)Eval_Expression(pexp->rhs, symbol));
				break;
			case RTEQ:	/* >= */
				reval = ((float)Eval_Expression(pexp->lhs, symbol) > (float)Eval_Expression(pexp->rhs, symbol)) 
					|| ((float)Eval_Expression(pexp->lhs, symbol) == (float)Eval_Expression(pexp->rhs, symbol));
				break;
			case NEQ:	reval = ((int)Eval_Expression(pexp->lhs, symbol) != (int)Eval_Expression(pexp->rhs, symbol)); break; /* != */
			case EQ:	reval = ((int)Eval_Expression(pexp->lhs, symbol) == (int)Eval_Expression(pexp->rhs, symbol)); break; /* == */
			case OR: reval = ((int)Eval_Expression(pexp->lhs, symbol).llVal || (int)Eval_Expression(pexp->rhs, symbol).llVal); break; /* || */
			case AND: reval = ((int)Eval_Expression(pexp->lhs, symbol).llVal && (int)Eval_Expression(pexp->rhs, symbol).llVal); break; /* && */
			case NEG: reval = (!(int)Eval_Expression(pexp->lhs, symbol).llVal); break;
			}
		}	
		break;

	case AddTermK: 
	case MulTermK:
		//reval =  visualizer::VariantCalc(GetAsciiFromTokentype(pexp->op), 
		//	Eval_Expression(pexp->lhs, symbol), Eval_Expression(pexp->rhs, symbol)); 
		break;

	case VariableK:
		reval = Eval_Variable(pexp, symbol);
		break;

	case IndirectK:
		break;

	case NumberK: reval = pexp->num; break;
	case StringK: reval = pexp->str.c_str(); break;
	}

	return reval;
}


//------------------------------------------------------------------------
// Variable �� �����Ѵ�.
//------------------------------------------------------------------------
_variant_t visualizer::Eval_Variable( SExpression *pexp, const SSymbolInfo &symbol )
{
	_variant_t reval;
	RETV(!pexp, reval);
	RETV(pexp->kind != VariableK, reval);

	SSymbolInfo findSymbol;
	if (!Find_Variable(pexp, symbol, &findSymbol))
		return reval;

	enum SymTagEnum symTag;
	HRESULT hr = findSymbol.pSym->get_symTag((DWORD*)&symTag);
	if (SymTagData == symTag) // constant data �ϰ�� ó��
	{
		DWORD dwLocType;
		hr = findSymbol.pSym->get_locationType(&dwLocType);
		if (LocIsConstant == dwLocType) // ��� 
			findSymbol.pSym->get_value(&reval);
	}

	if (reval.vt == VT_EMPTY)
		reval = dia::GetValueFromSymbol( findSymbol.mem.ptr, findSymbol.pSym);
		
	// ���߿� ó��
	switch (pexp->prefix_op)
	{
	case PLUS:
	case MINUS:
	case TIMES:
	case NEG:
	case INC: // ++
	case DEC: // --
		break;
	case REF: // &
		break;
	}

	return reval;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::Find_Variable( SExpression *pexp, 
							  IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut)
{
	bool reval = false;
	RETV(!pexp, false);
	RETV(pexp->kind != VariableK, false);

	if (pexp->str == "$e")
	{
		if (pexp->rhs)
		{
			reval = Find_Variable(pexp->rhs, symbol, pOut);
		}
		else
		{
			if (pOut)
			{
				pOut->isNotRelease = true; // symbol�� ���纻�̹Ƿ� release�ؼ� �ȵ�
				pOut->pSym = symbol.pSym;
				pOut->mem = SMemInfo( m_SymbolName.c_str(), m_MemInfo.ptr, 0 );
				reval = true;
			}
		}
	}
	else if (pexp->str == "$r")
	{

	}
	else if (pexp->str == "$i")
	{

	}
	else
	{
		if (pexp->rhs)
		{
			SSymbolInfo childSymbol;
			if (!Find_ChildSymbol(pexp->str, symbol, &childSymbol))
				return false;

			const string childName = dia::GetSymbolName(childSymbol.pSym);
			reval = Find_Variable(pexp->rhs, childSymbol, pOut);
		}
		else
		{
			reval = Find_ChildSymbol(pexp->str, symbol, pOut);
		}
	}

	return reval;
}


//------------------------------------------------------------------------
// symbol �� �ڽ����� findSymbolName �� �ش��ϴ� �ɺ���
// �ִٸ� pOut�� �����ؼ� �����Ѵ�. �� �� �ڽ��� �Ѵܰ踸
// ��������.
//------------------------------------------------------------------------
bool visualizer::Find_ChildSymbol(  const std::string findSymbolName,
								  IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut )
{
	RETV(!pOut, false);

	enum SymTagEnum symTag;
	HRESULT hr = symbol.pSym->get_symTag((DWORD*)&symTag);

	switch (symTag)
	{
	case SymTagData:
	case SymTagTypedef:
		{
			IDiaSymbol *pNewSymbol = NULL;
			HRESULT hr = symbol.pSym->get_type(&pNewSymbol);
			ASSERT_RETV(hr == S_OK, false);

			return Find_ChildSymbol( findSymbolName, SSymbolInfo(pNewSymbol, symbol.mem), pOut );
		}
		return true;

	case SymTagPointerType:
		{
			IDiaSymbol *pNewSymbol = NULL;
			HRESULT hr = symbol.pSym->get_type(&pNewSymbol);
			ASSERT_RETV(hr == S_OK, false);

			void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
			void *newPtr = MemoryMapping(srcPtr);
			if (newPtr) //�����޸𸮿��� ����� newPtr�� NULL�̵ȴ�.
			{
					return Find_ChildSymbol( findSymbolName,
						SSymbolInfo(pNewSymbol, SMemInfo(symbol.mem.name.c_str(), newPtr, 0)),
						pOut);
			}
		}
		return false;

	default:
		{
			LONG offset = 0;
			IDiaSymbol *pFindSymbol = dia::FindChildSymbol(findSymbolName, symbol.pSym, &offset);
			RETV(!pFindSymbol, false);

			if (pOut)
			{
				pOut->pSym = pFindSymbol;

				// memInfo
				pOut->mem.name = dia::GetSymbolName(pFindSymbol);
				pOut->mem.ptr = (BYTE*)symbol.mem.ptr + offset;
				pOut->mem.size = 0;
			}
		}
		return true;
	}
}


//------------------------------------------------------------------------
// ���б�ȣ�� �����Ѵ�.
//------------------------------------------------------------------------
char visualizer::GetAsciiFromTokentype(Tokentype tok)
{
	char c = NULL;
	switch (tok)
	{
	case PLUS: c = '+'; break;
	case MINUS: c = '-'; break;
	case TIMES: c = '*'; break;
	case DIV: c = '/'; break;
	}
	return c;
}


//------------------------------------------------------------------------
// ���� üũ
// condition �� true �̸� ���� ����
//------------------------------------------------------------------------
void visualizer::CheckError( bool condition, const string &msg ) // msg="error"
{
	if (condition) // success
		return;

	// error occur
	throw VisualizerScriptError( m_SymbolName + " " + msg);
}
