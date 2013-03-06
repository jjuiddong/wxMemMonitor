//------------------------------------------------------------------------
// Name:    Task.h
// Author:  jjuiddong
// Date:    2012-12-02
// 
// �����忡�� ����Ǵ� �½�ũ�� �����Ѵ�.
//------------------------------------------------------------------------
#pragma once

namespace common
{
	class CThread;
	class CTask
	{
	public:
		CTask(int id, const std::string &name="") : m_Id(id), m_pThread(NULL), m_Name(name) {}
		virtual ~CTask() {}

		enum RUN_RESULT
		{
			RR_END,			// �½�ũ ����
			RR_CONTINUE,	// �½�ũ ��ӽ���
		};

	protected:
		int					m_Id;
		CThread				*m_pThread;
		std::string			m_Name;

	public:
		int					GetId() const { return m_Id; }
		const std::string&	GetName() const { return m_Name; }
		void				SetThread(CThread *p) { m_pThread = p; }

		// overriding
		virtual RUN_RESULT	Run() { return RR_END; }
		virtual void		MessageProc( int msg, WPARAM wParam, LPARAM lParam ) {}

	};


	// list<CTask*>���� CTask�� ã�� ��ü
	class IsTask : public std::unary_function<CTask*, bool>
	{
	public:
		IsTask(int taskId):m_id(taskId) {}
		int m_id;
		bool operator ()(CTask *t) const
			{ return (t->GetId() == m_id); }
	};

}
