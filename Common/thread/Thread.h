//------------------------------------------------------------------------
// Name:    Thread.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// ������ ���̺귯��
// CTask �� �����Ѵ�.
//------------------------------------------------------------------------

#pragma once


namespace common
{
	class CTask;
	class CThread
	{
	public:
		CThread(const std::string &name="");
		virtual ~CThread();

		enum MSG_OPT
		{
			MSG_REMOVE,		// �޼��� ����
			MSG_KEEP,		// �޼��� ����
		};

		enum STATE
		{
			WAIT,			// �����
			RUN,			// ������
			END,			// ����
		};

	protected:
		STATE				m_State;
		HANDLE				m_hThread;
		std::string			m_Name;

		// �ܺ� �����忡�� �����ϱ� ������ ����ȭ �ڵ尡 �� �ִ�.
		CRITICAL_SECTION	m_TaskCriticalSection;
		TaskList			m_Tasks;

		CRITICAL_SECTION	m_MsgCriticalSection;
		ExternalMsgList		m_ThreadMsgs;				// �����尡 ���� �޼���
		ExternalMsgList		m_ExternalMsgs;				// �����忡�� �ܺη� ���� �޼���

	public:
		STATE			GetState() const { return m_State; }
		const std::string& GetName() const { return m_Name; }

		void			Start();						// ������ ����
		void			Terminate();					// ������ ����

		void			Send2ThreadMessage( int rcvTaskId, int msg, WPARAM wParam, LPARAM lParam);
		void			Send2ExternalMessage( int msg, WPARAM wParam, LPARAM lParam );
		bool			GetThreadMsg( OUT SExternalMsg *pMsg, MSG_OPT opt = MSG_REMOVE );
		bool			GetExternalMsg( OUT SExternalMsg *pMsg, MSG_OPT opt = MSG_REMOVE );
		
		bool			AddTask(CTask *pTask);
		bool			RemoveTask(CTask *pTask);
		void			Clear();

		void			Run();

		// Overriding
		virtual void	MessageProc( int msg, WPARAM wParam, LPARAM lParam ) {}

	protected:
		void			DispatchMessage();
		void			EnterTaskSync();
		void			LeaveTaskSync();
		void			EnterMsgSync();
		void			LeaveMsgSync();

	};

}
