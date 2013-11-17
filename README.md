wxMemMonitor
==========

memory monitor using wxWidgets (IDE: vs2010)


DIA SDK
----------------
- DIA Library Path
	- C:\Program Files\Microsoft Visual Studio 10.0\DIA SDK


wxMemMonitorLib
----------------


wxWidgets Setting
---------
- visual studio
 - general->mfc use - static mfc library
 - code generation->runtime library - multi thread debug 


Usage:
---------
```sh
#include "../wxMemMonitorLib/wxMemMonitor.h"
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


