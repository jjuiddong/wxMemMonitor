#include "stdafx.h"
#include "Thread.h"
#include "Task.h"
#include <process.h>
#include <boost/bind.hpp>

using namespace common;

namespace common
{
	unsigned __stdcall ThreadProcess(void *pThreadPtr )
	{
		CThread *pThread = (CThread*)pThreadPtr;
		pThread->Run();
		pThread->Exit();
		return 0;
	}

}


CThread::CThread(const std::string &name) :
	m_State(WAIT)
,	m_hThread(NULL)
//,	m_Name(name)
{
	strcpy_s(m_Name, name.c_str());
	InitializeCriticalSection( &m_TaskCriticalSection );
	InitializeCriticalSection( &m_MsgCriticalSection );

}

CThread::~CThread()
{
	Clear();

}


//------------------------------------------------------------------------
//  ������ ����
//------------------------------------------------------------------------
void CThread::Start()
{
	if (RUN != m_State)
	{
		m_State = RUN;
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProcess, this, 0, NULL);
	}
}


//------------------------------------------------------------------------
// ������ ����
//------------------------------------------------------------------------
void CThread::Terminate()
{
	m_State = END;
	WaitForSingleObject(m_hThread, 500); // �����尡 ����� ������ ��ٸ���.

}


//------------------------------------------------------------------------
// �����忡�� �޼����� ������.
// rcvTaskId : ���� �½�ũ ���̵� ('0' �̶�� �����尡 �޴´�.)
//			   -1 : �ܺη� ���� �޼����� ����
//------------------------------------------------------------------------
void CThread::Send2ThreadMessage( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added)
{
	EnterMsgSync();
	m_ThreadMsgs.push_back( SExternalMsg(-1, (int)msg, wParam, lParam, added) );
	LeaveMsgSync();
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void CThread::Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	EnterMsgSync();
	m_ExternalMsgs.push_back( SExternalMsg(-1, msg, wParam, lParam, added) );
	LeaveMsgSync();
}


//------------------------------------------------------------------------
// Task ����ȭ
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
// Msg ����ȭ
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
// �����尡 ���� �޼����� �����Ѵ�.
// �޼����� ���ٸ� false�� �����Ѵ�.
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
// �����忡�� �ܺη� ���ϴ� �޼����� �����Ѵ�.
// �޼����� ���ٸ� false�� �����Ѵ�.
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
			return false; // �̹� �����Ѵٸ� ����

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
			return false; // ���ٸ� ����

		m_Tasks.remove_if( IsTask(pTask->GetId()) );
	}
	LeaveTaskSync();
	return true;
}


//------------------------------------------------------------------------
// �������� ������ Ŭ���� ����
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
// ������ ����
// Task�� �����Ų��.
//------------------------------------------------------------------------
void CThread::Run()
{
	while (RUN == m_State)
	{
		if (m_Tasks.empty()) // break no task
			break;

		//1. �½�ũ ó��
		EnterTaskSync();
		{
			TaskItor it = m_Tasks.begin();
			while (m_Tasks.end() != it)
			{
				CTask *pTask = *it;
				if (CTask::RR_END == pTask->Run())
				{
					// �½�ũ ����
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

		//2. �޼��� ó��
		DispatchMessage();

		Sleep(1);
	}

	// ���������� �� �޼����� ���������� ó���Ѵ�.
	DispatchMessage();
}


//------------------------------------------------------------------------
// call exit thread
//------------------------------------------------------------------------
void	CThread::Exit()
{
	m_State = END;
}


//------------------------------------------------------------------------
// ����� �޼������� �½�ũ�� ������.
//------------------------------------------------------------------------
void CThread::DispatchMessage()
{
	EnterMsgSync();
	{
		ExternalMsgItor it = m_ThreadMsgs.begin();
		while (m_ThreadMsgs.end() != it)
		{
			if (threadmsg::TASK_MSG == it->msg) // task message
			{
				TaskItor t = find_if(m_Tasks.begin(), m_Tasks.end(), 
					boost::bind( &IsSameId<CTask>, _1, it->wParam) );
				if (m_Tasks.end() != t)
				{
					(*t)->MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
				}
				else
				{
					// �������� ���� �޼��� error
				}
			}
			else // Thread���� �� �޼���
			{
				MessageProc((threadmsg::MSG)it->msg, it->wParam, it->lParam, it->added);
			}
			++it;
		}
		m_ThreadMsgs.clear();
	}
	LeaveMsgSync();
}


//------------------------------------------------------------------------
// Message Process
//------------------------------------------------------------------------
void	CThread::MessageProc( threadmsg::MSG msg, WPARAM wParam, LPARAM lParam, LPARAM added )
{
	switch (msg)
	{
	case threadmsg::TERMINATE_TASK:
		{
			// terminate task of id wParam
			EnterTaskSync();
			{
				auto it = std::find_if( m_Tasks.begin(), m_Tasks.end(), 
					bind( &IsSameId<common::CTask>, _1, (int)wParam) );
				if (m_Tasks.end() != it)
				{
					delete *it;
					m_Tasks.erase(it);
				}
			}
			LeaveTaskSync();
		}
		break;
	}
}
