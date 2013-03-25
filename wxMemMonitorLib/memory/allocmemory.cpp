
#include "stdafx.h"
#include "../wxMemMonitor.h"
#include "../Control/Global.h"
#include "SharedMemoryMng.h"


namespace memmonitor
{
	void*	AllocateLocal(const std::string &name, size_t size);
	bool		DeAllocateLocal(void *ptr);

	struct SMemData
	{
		SMemData() : ptr(NULL) {}
		SMemData(void *_ptr) : ptr(_ptr) {}
		void *ptr;
	};

	typedef  std::map<std::string, SMemData> MapType;
	MapType g_MemMap;
}


using namespace memmonitor;

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void* memmonitor::Allocate(const std::string &name, size_t size)
{
	void *ptr = NULL;
	switch (GetExecuteType())
	{
	case INNER_PROCESS: ptr = AllocateLocal(name, size); break;
	case OUTER_PROCESS: ptr = sharedmemory::Allocate(name, size); break;
	}
	return ptr;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool memmonitor::DeAllocateMem(void *ptr)
{
	bool result = false;
	switch (GetExecuteType())
	{
	case INNER_PROCESS: result = DeAllocateLocal(ptr); break;
	case OUTER_PROCESS: result = sharedmemory::DeAllocate(ptr); break;
	}
	return result;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void* memmonitor::AllocateLocal(const std::string &name, size_t size)
{
	auto it = g_MemMap.find(name);
	if (g_MemMap.end() != it)
		return NULL; // already exist

	void *ptr = ::malloc(size);
	if (!ptr) // Error!!
		return NULL;

	g_MemMap.insert( MapType::value_type(name, SMemData(ptr)) );
	return ptr;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool memmonitor::DeAllocateLocal(void *ptr)
{
	BOOST_FOREACH(auto &it, g_MemMap)
	{
		if (it.second.ptr == ptr)
		{
			::free(ptr);
			g_MemMap.erase(it.first);
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------
// enumerate memory info
//------------------------------------------------------------------------
void	memmonitor::EnumerateMemoryInfo(OUT MemoryList &memList)
{
	switch (GetExecuteType())
	{
	case INNER_PROCESS:
		{
			BOOST_FOREACH(auto &it, g_MemMap)
			{
				memList.push_back( SMemInfo(it.first.c_str(), it.second.ptr, 0) );
			}
		}
		break;

	case OUTER_PROCESS:
		{
			sharedmemory::MemoryList mems;
			sharedmemory::EnumerateMemoryInfo(mems);
			BOOST_FOREACH(auto &it, mems)
			{
				memList.push_back(SMemInfo(it.name.c_str(), it.ptr, it.size));
			}
		}
		break;
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	memmonitor::FindMemoryInfo(const std::string &name, OUT SMemInfo &info)
{	
	switch (GetExecuteType())
	{
	case INNER_PROCESS:
		{
			auto it = g_MemMap.find(name);
			if (g_MemMap.end() == it)
				return false;
			info = SMemInfo(name.c_str(), it->second.ptr, 0);
		}
		break;

	case OUTER_PROCESS:
		{
			sharedmemory::SMemoryInfo meminfo;
			if (!sharedmemory::FindMemoryInfo(name, meminfo))
				return false;
			info = SMemInfo(meminfo.name.c_str(), meminfo.ptr, meminfo.size);
		}
		break;
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void* memmonitor::MemoryMapping(void *srcPtr )
{
	switch (GetExecuteType())
	{
	case INNER_PROCESS: return srcPtr;
	case OUTER_PROCESS: return sharedmemory::MemoryMapping(srcPtr);
	}
	return NULL;
}


//------------------------------------------------------------------------
// TypeName%Count 형태로 되어있는 타입이름을 TypeName만 리턴하게 한다.
//------------------------------------------------------------------------
std::string memmonitor::ParseObjectName(const std::string &objectName)
{
	const int offset = objectName.find('#');
	if (std::string::npos == offset)
		return objectName;
	else
		return objectName.substr(0,offset);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	memmonitor::CheckValidAddress(const void *ptr )
{
	switch (GetExecuteType())
	{
	case INNER_PROCESS: return true;
	case OUTER_PROCESS: return sharedmemory::CheckValidAddress(ptr);
	}
	return false;
}
