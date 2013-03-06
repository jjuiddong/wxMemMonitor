//------------------------------------------------------------------------
// Name:    Reference.h
// Author:  jjuiddong
// Date:    12/22/2012
// 
// ���ڷ� �Ѿ�� Ÿ���� �����͸� ��������� ������.
//
// �ٸ� ������ �Ҵ�� �޸� �����͸� ������ �� ����ϴ� Ŭ������.
// �Ǽ��� �޸𸮸� �����ϴ� ���� �������� ���������.
//
// delete �����ڷ� ReferencePtr�� ������ �� �ִ� ������ �ִ�. 
// T* operator�� �����ؾ���
//------------------------------------------------------------------------
#pragma once

namespace common
{
	template <class T>
	class ReferencePtr
	{
	public:
		ReferencePtr():m_p(NULL) {}
		ReferencePtr(T *p) : m_p(p) {}
	private:
		typedef ReferencePtr<T> this_type;
	protected:
		T *m_p;
	public:
		typedef T* this_type::*unspecified_bool_type;

		T* operator->() const { return m_p; }
		bool operator!() const { return (m_p)? false : true; }
		T* operator=(T *p) { m_p = p; return m_p; }
		bool operator==(T *p) { return m_p == p; }
		bool operator==(const ReferencePtr<T> &rhs) { return m_p == rhs.m_p; }
		operator T*() { return m_p; } // casting operator
// 		operator unspecified_bool_type() const 
// 		{ 
// 			return (m_p == NULL)?  0 : &this_type::m_p;
// 		} // casting operator

	};
}
