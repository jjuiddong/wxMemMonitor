//------------------------------------------------------------------------
// Name:    VisualizerGlobal.h
// Author:  jjuiddong
// Date:    2/4/2013
// 
// visualizer ���� ����ϴ� ���� ���ǵ� ����
//------------------------------------------------------------------------
#ifndef __VISUALIZERGLOBAL_H__
#define __VISUALIZERGLOBAL_H__


struct IDiaSymbol;
namespace visualizer
{
	using namespace sharedmemory;

	struct SSymbolInfo
	{
		IDiaSymbol *pSym;
		SMemoryInfo mem;	
		bool isNotRelease; // defalut : false

		SSymbolInfo() 
		{ 
			pSym = NULL; 
			isNotRelease = false;
		}
		SSymbolInfo( const SSymbolInfo &rhs );
		SSymbolInfo( IDiaSymbol *psym, const SMemoryInfo &_mem) 
			: pSym(psym), mem(_mem), isNotRelease(false)
		{ 
		}
		SSymbolInfo( IDiaSymbol *psym, const SMemoryInfo &_mem, bool _isRelease) 
			: pSym(psym), mem(_mem), isNotRelease(!_isRelease)
		{
		}
		~SSymbolInfo(); // �Ҹ��ڿ��� pSym ������ �����Ѵ�. isNotRelease�� false�϶���

		SSymbolInfo& operator= (const SSymbolInfo &rhs);
		
	};

}

#endif // __VISUALIZERGLOBAL_H__
