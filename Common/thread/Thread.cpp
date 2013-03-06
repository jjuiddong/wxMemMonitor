#include "stdafx.h"
#include "Thread.h"
#include "Task.h"
#include <process.h>


using namespace common;

namespace common
{
	unsigned __stdcall ThreadProcess(void *pThreadPtr )
	{
		CThread *pThread = (CThread*)pThreadPtr;
		pThread->Run();
		return 0;
	}

}


CThread::CThread(const std::string &name) :
	m_State(WAIT)
,	m_hThread(NULL)
,	m_Name(name)
{
	InitializeCriticalSection( &m_TaskCriticalSection );
	InitializeCriticalSection( &m_MsgCriticalSection );

}

CThread::~CThread()
{
	Clear();

}


//------------------------------------------------------------------------
//  쓰레드 실행
//------------------------------------------------------------------------
void CThread::Start()
{
	// 쓰레드 실행
	m_State = RUN;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProcess, this, 0, NULL);
}


//------------------------------------------------------------------------
// 쓰레드 종료
//------------------------------------------------------------------------
void CThread::Terminate()
{
	m_State = END;
	WaitForSingleObject(m_hThread, 500); // 쓰레드가 종료될 때까지 기다린다.

}


//------------------------------------------------------------------------
// 쓰레드에게 메세지를 보낸다.
// rcvTaskId : 받을 태스크 아이디 ('0' 이라면 쓰레드가 받는다.)
//			   -1 : 외부로 가는 메세지를 뜻함
//------------------------------------------------------------------------
void CThread::Send2ThreadMessage( int rcvTaskId, int msg, WPARAM wParam, LPARAM lParam)
{
	EnterMsgSync();
	m_ThreadMsgs.push_back( SExternalMsg(rcvTaskId, msg, wParam, lParam) );
	LeaveMsgSync();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CThread::Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam )
{
	EnterMsgSync();
	m_ExternalMsgs.push_back( SExternalMsg(-1, msg, wParam, lParam) );
	LeaveMsgSync();
}


//------------------------------------------------------------------------
// Task 동기화
//------------------------------------------------------------------------
void CThread::EnterTaskSync()
{
	EnterCriticalSection( &m_TaskCriticalSection );
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CThread::LeaveTaskSync()
{
	LeaveCriticalSection( &m_TaskCriticalSection );
}
//------------------------------------------------------------------------
// Msg 동기화
//------------------------------------------------------------------------
void CThread::EnterMsgSync()
{
	EnterCriticalSection( &m_MsgCriticalSection );
}
//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CThread::LeaveMsgSync()
{
	LeaveCriticalSection( &m_MsgCriticalSection );
}


//------------------------------------------------------------------------
// 쓰레드가 받은 메세지를 리턴한다.
// 메세지가 없다면 false를 리턴한다.
//------------------------------------------------------------------------
bool CThread::GetThreadMsg( OUT SExternalMsg *pMsg, MSG_OPT opt ) // opt = MSG_REMOVE
{
	if (!pMsg) 
		return false;

	bool reval;
	EnterMsgSync();
	{
		if (m_ThreadMsgs.empty())
		{
			reval = false;
		}
		else
		{
			*pMsg = m_ThreadMsgs.front();
			if (MSG_REMOVE == opt)
				m_ThreadMsgs.pop_front();
			reval = true;
		}
	}
	LeaveMsgSync();
	return reval;
}


//------------------------------------------------------------------------
// 쓰레드에서 외부로 향하는 메세지를 리턴한다.
// 메세지가 없다면 false를 리턴한다.
//------------------------------------------------------------------------
bool CThread::GetExternalMsg( OUT SExternalMsg *pMsg, MSG_OPT opt ) // opt = MSG_REMOVE
{
	if (!pMsg) 
		return false;

	bool reval;
	EnterMsgSync();
	{
		if (m_ExternalMsgs.empty())
		{
			reval = false;
		}
		else
		{
			*pMsg = m_ExternalMsgs.front();
			if (MSG_REMOVE == opt)
				m_ExternalMsgs.pop_front();
			reval = true;
		}
	}
	LeaveMsgSync();
	return reval;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CThread::AddTask(CTask *pTask)
{
	if (!pTask)
		return false;
	EnterTaskSync();
	{
		TaskItor it = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(pTask->GetId()));
		if (m_Tasks.end() != it)
			return false; // 이미 존재한다면 실패

		pTask->SetThread(this);
		m_Tasks.push_back( pTask );
	}
	LeaveTaskSync();
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool CThread::RemoveTask(CTask *pTask)
{
	if (!pTask)
		return false;
	EnterTaskSync();
	{
		TaskItor it = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(pTask->GetId()));
		if (m_Tasks.end() == it)
			return false; // 없다면 실패

		m_Tasks.remove_if( IsTask(pTask->GetId()) );
	}
	LeaveTaskSync();
	return true;
}


//------------------------------------------------------------------------
// 동적으로 생성된 클래스 제거
//------------------------------------------------------------------------
void CThread::Clear()
{
	EnterTaskSync();
	{
		TaskItor it = m_Tasks.begin();
		while (m_Tasks.end() != it)
		{
			CTask *p = *it++;
			delete p;
		}
		m_Tasks.clear();
	}
	LeaveTaskSync();

	EnterMsgSync();
	{
		m_ThreadMsgs.clear();
		m_ExternalMsgs.clear();
	}
	LeaveMsgSync();

	DeleteCriticalSection( &m_TaskCriticalSection );
	DeleteCriticalSection( &m_MsgCriticalSection );

	CloseHandle(m_hThread);
}


//------------------------------------------------------------------------
// 쓰레드 실행
// Task를 실행시킨다.
//------------------------------------------------------------------------
void CThread::Run()
{
	while (RUN == m_State)
	{
		//1. 태스크 처리
		EnterTaskSync();
		{
			TaskItor it = m_Tasks.begin();
			while (m_Tasks.end() != it)
			{
				CTask *pTask = *it;
				if (CTask::RR_END == pTask->Run())
				{
					// 태스크 제거
					it = m_Tasks.erase(it);
					delete pTask;
				}
				else
				{
					++it;
				}
			}
		}
		LeaveTaskSync();

		//2. 메세지 처리
		DispatchMessage();

		Sleep(1);
	}

	// 남았을지도 모를 메세지를 마지막으로 처리한다.
	DispatchMessage();
}


//------------------------------------------------------------------------
// 저장된 메세지들을 태스크로 보낸다.
//------------------------------------------------------------------------
void CThread::DispatchMessage()
{
	EnterMsgSync();
	{
		ExternalMsgItor it = m_ThreadMsgs.begin();
		while (m_ThreadMsgs.end() != it)
		{
			if (0 == it->rcvTaskId) // Thread에게 온 메세지
			{
				MessageProc(it->msg, it->wParam, it->lParam);
			}
			else
			{
				TaskItor t = find_if(m_Tasks.begin(), m_Tasks.end(), IsTask(it->rcvTaskId));
				if (m_Tasks.end() != t)
				{
					(*t)->MessageProc(it->msg, it->wParam, it->lParam);
				}
				else
				{
					// 목적지가 없는 메세지 error
				}
			}
			++it;
		}
		m_ThreadMsgs.clear();
	}
	LeaveMsgSync();
}
