
#include "stdafx.h"
#include "FilePath.h"
#include <shlwapi.h> // 이 헤더 파일에 FilePath에 관련된 함수들이 많다. 잘 이용해보자.
#pragma comment(lib, "shlwapi")

using namespace common;

//	_splitpath(findFilePath.c_str(), drive, dir, NULL,NULL); 함수로 파일명을 분리할 수 있다.


//------------------------------------------------------------------------
// fileName의 경로와 확장자를 제외한 파일이름만을 리턴한다.
//------------------------------------------------------------------------
std::string common::GetFileNameExceptExt(const std::string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	char *name = PathFindFileNameA(srcFileName);
	PathRemoveExtensionA(name);
	return name;
}


//------------------------------------------------------------------------
// fileName에서 파일이름과 확장자를 제외한 나머지 경로를 리턴한다.
//------------------------------------------------------------------------
std::string common::GetFilePathExceptFileName(const std::string &fileName)
{
	char srcFileName[ MAX_PATH];
	strcpy_s(srcFileName, MAX_PATH, fileName.c_str() );
	PathRemoveFileSpecA( srcFileName );
	return srcFileName;
}
