
#include <iostream>

#include "../wxMemMonitorLib/wxMemMonitor.h"
MEMORYMONITOR_INNER_PROCESS();

using namespace std;

DECLARE_TYPE_NAME(CTest)
class CTest : public memmonitor::Monitor<CTest, TYPE_NAME(CTest)>
{
public:
	string str;
};

int main()
{
	memmonitor::Init(memmonitor::INNER_PROCESS, NULL, "config_target2.json" );

	cout << "target2\n";

	CTest *t = new CTest();
	t->str = "test";
	cout << t->str << endl;

	int nn = t->str._BUF_SIZE;

	string str2;
	cin >> str2;
	cout << str2 << endl;

	return 0;
}
