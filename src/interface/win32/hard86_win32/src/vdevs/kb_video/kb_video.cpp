// kb_video.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <queue>
using namespace std;

extern HMODULE hModule;
HANDLE hMainThread;
HANDLE hIntThread;
HANDLE hHasTextEvent;
HWND hWndMain;

const int CHAR_W	= 8;
const int CHAR_H	= 16;
const int SCREEN_W	= CHAR_W * 80;
const int SCREEN_H	= CHAR_H * 25;

queue<char> textBuffer;

bool bExiting=false;

sys_state_ptr sysState=NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	switch(uMsg){
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		textBuffer.push((char)wParam);
		SetEvent(hHasTextEvent);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		{
			/*
			hDC=BeginPaint(hWnd, &ps);
			HGDIOBJ hOldFont=SelectObject(hDC, (HGDIOBJ)GetStockObject(OEM_FIXED_FONT));
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			SetBkColor(hDC, RGB(0, 0, 0));
			SetTextColor(hDC, RGB(255, 255, 255));
			DrawTextA(hDC, ????, -1, &clientRect, NULL);
			SelectObject(hDC, hOldFont);
			EndPaint(hWnd, &ps);
			*/
		}
		break;
	case WM_DESTROY:
		::bExiting=true;
		PostQuitMessage(WM_QUIT);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

DWORD CALLBACK IntThread(LPVOID lpParam){
	while(!bExiting){
		WaitForSingleObject(hHasTextEvent, INFINITE);
		// we have text now, make an interrupt call
		if(::sysState){
			write_io_port(::sysState, (uint8)textBuffer.front(), 0x60);
			textBuffer.pop();
			make_int_call(::sysState, 0x7);
			if(textBuffer.empty()){
				ResetEvent(hHasTextEvent);
			}
		}else{
			ResetEvent(hHasTextEvent);
		}
	}
	return 0;
}

DWORD CALLBACK MainThread(LPVOID lpParam){
	// Register window
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(WNDCLASSEX);
	wcx.style=CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	wcx.lpfnWndProc=&WndProc;
	wcx.cbClsExtra=NULL;
	wcx.cbWndExtra=NULL;
	wcx.hInstance=::hModule;
	wcx.hIcon=NULL;
	wcx.hCursor=(HCURSOR)LoadCursor(::hModule, IDC_ARROW);
	wcx.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wcx.lpszMenuName=NULL;
	wcx.lpszClassName=L"MyClass";
	wcx.hIconSm=NULL;
	RegisterClassEx(&wcx);

	// Create window
	hWndMain=CreateWindowEx(NULL,
						L"MyClass",
						L"Window",
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT,
						SCREEN_W, SCREEN_H,
						NULL, NULL,
						::hModule, NULL);
	ShowWindow(hWndMain, SW_SHOW);

	hHasTextEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
	CreateThread(NULL, NULL, IntThread, NULL, NULL, NULL);

	MSG msg;
	while(GetMessage(&msg, NULL, NULL, NULL)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

int VirtualDevice_AcceptEmulationMutex(MUTEX a, sys_state_ptr b)
{
	sysState=b;
	return 0;
}

int VirtualDevice_Terminate()
{
	// Destroy window here
	WaitForSingleObject(hMainThread, INFINITE);
	return 0;
}

int VirtualDevice_Initialize(void* a, void* b)
{
	hMainThread=CreateThread(NULL, NULL, MainThread, NULL, NULL, NULL);
#ifdef BUILD_EXE
	WaitForSingleObject(hMainThread, INFINITE);
#endif
	return 0;
}
