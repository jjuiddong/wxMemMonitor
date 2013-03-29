
#include <iostream>
#include <list>

#include "../wxMemMonitorLib/wxMemMonitor.h"
MEMORYMONITOR_INNER_PROCESS();

using namespace std;

class CSubClass 
{
public:
	CSubClass() {}
	CSubClass(int i) : a(i) {}
	int a;
	float b;
	char c;
};

DECLARE_TYPE_NAME(CMemTest)
class CMemTest : public memmonitor::Monitor<CMemTest, TYPE_NAME(CMemTest)>
{
public:
	string str;
	std::list<int> m_list;
	std::list<CSubClass> m_sublist;
	std::list<CSubClass*> m_subPtrlist;
};


int main()
{
	memmonitor::Init(memmonitor::INNER_PROCESS, NULL, "config_target2.json" );

	cout << "target2\n";

	CMemTest *t = new CMemTest();
	t->str = "test";
	cout << t->str << endl;

	t->m_list.push_back(1);
	t->m_list.push_back(2);
	t->m_list.push_back(3);
	t->m_list.push_back(4);

	t->m_sublist.push_back( CSubClass(1) );
	t->m_sublist.push_back( CSubClass(2) );
	t->m_sublist.push_back( CSubClass(3) );

	t->m_subPtrlist.push_back( new CSubClass(100) );
	t->m_subPtrlist.push_back( new CSubClass(200) );
	t->m_subPtrlist.push_back( new CSubClass(300) );

	CMemTest *p2 = new CMemTest();
	p2->str = "test";
	cout << p2->str << endl;

	string str2;
	cin >> str2;
	cout << str2 << endl;

	return 0;
}
