wxMemMonitor
==========

memory monitor with wxWidgets, DIA SDK (IDE: vs2010)


DIA SDK
----------------
- DIA Library Path
	- C:\Program Files\Microsoft Visual Studio 10.0\DIA SDK


wxMemMonitorLib
----------------


wxWidgets Build Setting


Usage:
---------
```sh
#include "../wxMemMonitorLib/wxMemMonitor.h"
MEMORYMONITOR_INNER_PROCESS();
if (!memmonitor::Init(memmonitor::INNER_PROCESS,hInstance,"config_target.json" ))
{
    MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
}
~~
~~
memmonitor::Cleanup();
```

* ambigious funciton 'floor' 
	- modify -> floor to ::floor (in wxWidgets/geometry.h)


