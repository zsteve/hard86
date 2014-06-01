// kb_video.cpp : Defines the exported functions for the DLL application.
//

/*	Base video buffer address :	c800:0000 (0xfa00 bytes)
	Hardware I/O port register:	0x41
								nonzero -> buffer not locked
								zero	-> buffer locked
*/

#include "stdafx.h"
#include <string>
#include <queue>

using namespace std;

extern HMODULE hModule;
HANDLE hMainThread;

HWND hWndMain;
sys_state_ptr sysState=NULL;

HDC hLCD;
HBITMAP hLCDBmp;

const int SCREEN_W=320, SCREEN_H=200;

uint8 screenBuffer[SCREEN_H][SCREEN_W];

bool wantData=false;

int RectWidth(const RECT& rc){
	return (rc.right-rc.left);
}

int RectHeight(const RECT& rc){
	return (rc.bottom-rc.top);
}

int WindowWidth(HWND hWnd){
	static RECT rc;
	GetWindowRect(hWnd, &rc);
	return RectWidth(rc);
}

int WindowHeight(HWND hWnd){
	static RECT rc;
	GetWindowRect(hWnd, &rc);
	return RectHeight(rc);
}

int ClientWidth(HWND hWnd){
	static RECT rc;
	GetClientRect(hWnd, &rc);
	return RectWidth(rc);
}

int ClientHeight(HWND hWnd){
	static RECT rc;
	GetClientRect(hWnd, &rc);
	return RectHeight(rc);
}

int WindowX(HWND hWnd){
	static RECT rc;
	GetWindowRect(hWnd, &rc);
	return (rc.left);
}

int WindowY(HWND hWnd){
	static RECT rc;
	GetWindowRect(hWnd, &rc);
	return (rc.top);
}

int ClientX(HWND hWnd){
	static RECT rc;
	GetClientRect(hWnd, &rc);
	return (rc.left);
}

int ClientY(HWND hWnd){
	static RECT rc;
	GetClientRect(hWnd, &rc);
	return (rc.top);
}

int SetWindowSize(HWND hWnd, int w, int h){
	return MoveWindow(hWnd, WindowX(hWnd), WindowY(hWnd), w, h, true);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	switch(uMsg){
	case WM_CREATE:
		SelectObject(hLCD, (HGDIOBJ)hLCDBmp);
		SetTimer(hWnd, 0, 30, NULL);
		break;
	case WM_TIMER:
		wantData=true;
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_CHAR:

		break;
	case WM_PAINT:
		{
			hDC=BeginPaint(hWnd, &ps);
			hLCD=CreateCompatibleDC(hDC);
			hLCDBmp=CreateCompatibleBitmap(hLCD, SCREEN_W, SCREEN_H);
			SelectObject(hLCD, (HGDIOBJ)hLCDBmp);
			PatBlt(hLCD, 0, 0, SCREEN_W, SCREEN_H, WHITENESS);
			for(int i=0; i<SCREEN_H; i++){
				for(int j=0; j<SCREEN_W; j++){
					if(screenBuffer[i][j]){
						SetPixel(hLCD, j, i, RGB(0, 0, 0));
					}
				}
			}
			StretchBlt(hDC, 0, 0, SCREEN_W*2, SCREEN_H*2, hLCD, 0, 0, SCREEN_W, SCREEN_H, SRCCOPY);
			DeleteDC(hLCD);
			DeleteObject((HGDIOBJ)hLCDBmp);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(WM_QUIT);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
	wcx.lpszClassName="MyClass";
	wcx.hIconSm=NULL;
	RegisterClassEx(&wcx);

	// Create window

	hWndMain=CreateWindowEx(NULL,
						"MyClass",
						"Virtual LCD Screen - 320x200 - Buffer at c800:0000",
						WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT,
						650, 440,
						NULL, NULL,
						::hModule, NULL);
	
	ShowWindow(hWndMain, SW_SHOW);
	SetWindowSize(hWndMain, 640+(WindowWidth(hWndMain)-ClientWidth(hWndMain)),
						320+(WindowHeight(hWndMain)-ClientHeight(hWndMain)));
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
	if(::wantData){
		if(read_io_port(b, 0x41)){
			uint32 baseAddr=0xc8000;
			for(int i=0; i<SCREEN_H; i++){
				for(int j=0; j<SCREEN_W; j++){
					::screenBuffer[i][j]=read_mem_8(b, baseAddr+(i*SCREEN_W)+j);
				}
			}
			wantData=false;
		}
	}
	return 0;
}

int VirtualDevice_Terminate()
{
	// Destroy window here
	SendMessage(hWndMain, WM_CLOSE, NULL, NULL);
	WaitForSingleObject(hMainThread, INFINITE);
	return 0;
}

#define BUILD_EXE

int VirtualDevice_Initialize(void* a, void* b)
{
	hMainThread=CreateThread(NULL, NULL, MainThread, NULL, NULL, NULL);
#ifndef _USRDLL
	WaitForSingleObject(hMainThread, INFINITE);
#endif
	return 0;
}
