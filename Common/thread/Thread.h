//------------------------------------------------------------------------
// Name:    Thread.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// 쓰레드 라이브러리
// CTask 를 실행한다.
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
			MSG_REMOVE,		// 메세지 제거
			MSG_KEEP,		// 메세지 보존
		};

		enum STATE
		{
			WAIT,			// 대기중
			RUN,			// 실행중
			END,			// 종료
		};

	protected:
		STATE				m_State;
		HANDLE				m_hThread;
		std::string			m_Name;

		// 외부 쓰레드에서 접근하기 때문에 동기화 코드가 들어가 있다.
		CRITICAL_SECTION	m_TaskCriticalSection;
		TaskList			m_Tasks;

		CRITICAL_SECTION	m_MsgCriticalSection;
		ExternalMsgList		m_ThreadMsgs;				// 쓰레드가 받은 메세지
		ExternalMsgList		m_ExternalMsgs;				// 쓰레드에서 외부로 보낸 메세지

	public:
		STATE			GetState() const { return m_State; }
		const std::string& GetName() const { return m_Name; }

		void			Start();						// 쓰레드 실행
		void			Terminate();					// 쓰레드 종료

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
