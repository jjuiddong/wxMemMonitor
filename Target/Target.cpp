// Target.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <math.h>
#include <vector>
#include "Target.h"
#include "Ball.h"


#include "../wxMemMonitorLib/wxMemMonitor.h"
MEMORYMONITOR_INNER_PROCESS();



#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
HWND g_hWnd;
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

std::vector<CBall*> m_Balls;
const int MAX_BALL_COUNT = 30;
const int DEFAULT_RADIUS = 10;
HBRUSH g_Brush[ 4];

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void					MainLoop(int elapse_time);
void					Render(HWND hWnd);
void					Paint(HWND hWnd, HDC hdc);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TARGET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TARGET));

	//if (!memmonitor::Init( memmonitor::INNER_PROCESS, hInstance, "config_bounceball.json" ))
	//if (!memmonitor::Init( memmonitor::INNER_PROCESS, hInstance, "config_madsoccer_server.json" ))
	if (!memmonitor::Init( memmonitor::INNER_PROCESS, hInstance, "config_target.json" ))
	{
		MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
	}

	for (int i=0; i < MAX_BALL_COUNT; ++i)
	{
		POINT pt = {(i+1)*30, (i+1)*30};
		std::stringstream ss;
		ss << "Ball " << i+1;
		m_Balls.push_back( new CBall(ss.str(), pt, DEFAULT_RADIUS));
	}

	g_Brush[ 0] = CreateSolidBrush( RGB(255,255,0) );
	g_Brush[ 1] = CreateSolidBrush( RGB(0,255,0) );
	g_Brush[ 2] = CreateSolidBrush( RGB(0, 0, 255) );
	g_Brush[ 3] = CreateSolidBrush( RGB(255,0,0) );

	// �⺻ �޽��� �����Դϴ�.
	int oldT = GetTickCount();
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const int curT = GetTickCount();
		const int elapseT = min(curT - oldT, 100);
		oldT = curT;
		MainLoop(elapseT);	
	}

	for (int i=0; i < MAX_BALL_COUNT; ++i)
		delete m_Balls[ i];
	DeleteObject(g_Brush[ 0] );
	DeleteObject(g_Brush[ 1] );
	DeleteObject(g_Brush[ 2] );
	DeleteObject(g_Brush[ 3] );

	memmonitor::Cleanup();
	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TARGET));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TARGET);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 400, NULL, NULL, hInstance, NULL);
   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �޴� ������ ���� �м��մϴ�.
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Paint(hWnd, ps.hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_KEYDOWN:
		if (VK_ESCAPE == wParam)
			PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


/**
 @brief 
 */
void	MainLoop(int elapse_time)
{
	// Move
	RECT cr;
	::GetClientRect(g_hWnd, &cr);
	for (size_t i=0; i < m_Balls.size(); ++i)
		m_Balls[ i]->Move( cr, m_Balls, elapse_time );

	// Render
	Render(g_hWnd);
	::InvalidateRect(g_hWnd, NULL, TRUE);
}


/**
 @brief 
 */
void	Render(HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	Paint(hWnd, hdc);
	::ReleaseDC(hWnd, hdc);
}


/**
 @brief 
 */
void Paint(HWND hWnd, HDC hdc)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right-rc.left, rc.bottom-rc.top);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
	HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(hdcMem, &rc, hbrBkGnd);
	DeleteObject(hbrBkGnd);

	for(size_t i=0; i < m_Balls.size(); ++i)
	{
		RECT r = m_Balls[ i]->GetRect();
		SelectObject(hdcMem, g_Brush[ m_Balls[ i]->GetColor()] );
		::Ellipse(hdcMem,  r.left, r.top, r.right, r.bottom);
	}

	BitBlt(hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
}
