// kb_video.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <queue>

using namespace std;

extern HMODULE hModule;
HANDLE hMainThread;

HWND hWndMain;
sys_state_ptr sysState=NULL;

HBITMAP ledOn, ledOff;
union{
	uint8 val;
	struct{
		int bit0:1;
		int bit1:1;
		int bit2:1;
		int bit3:1;
		int bit4:1;
		int bit5:1;
		int bit6:1;
		int bit7:1;
	};
}portVal;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	switch(uMsg){
	case WM_CREATE:
		ledOn=LoadBitmap(hModule, MAKEINTRESOURCE(IDB_ON));
		ledOff=LoadBitmap(hModule, MAKEINTRESOURCE(IDB_OFF));

		portVal.val=0;

		SetTimer(hWnd, 0, 30, NULL);
		break;
	case WM_TIMER:
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_CHAR:

		break;
	case WM_PAINT:
		{
			hDC=BeginPaint(hWnd, &ps);
			HDC hDCOnLed=CreateCompatibleDC(hDC);
			HDC hDCOffLed=CreateCompatibleDC(hDC);

			HGDIOBJ hOldBitmap1=SelectObject(hDCOnLed, (HGDIOBJ)ledOn);
			HGDIOBJ hOldBitmap2=SelectObject(hDCOffLed, (HGDIOBJ)ledOff);

			int x=0;

			if(portVal.bit7)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit6)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit5)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit4)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit3)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit2)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit1)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			if(portVal.bit0)
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOnLed, 0, 0, SRCCOPY);
			else
				BitBlt(hDC, x+=16, 0, 16, 16, hDCOffLed, 0, 0, SRCCOPY);

			char valStr[3];
			itoa(portVal.val, valStr, 16);

			SetTextColor(hDC, RGB(255, 255, 255));
			SetBkColor(hDC, RGB(0, 0, 0));
			x+=32;
			TextOut(hDC, x, 0, valStr, 2);

			SelectObject(hDCOnLed, hOldBitmap1);
			SelectObject(hDCOffLed, hOldBitmap2);

			DeleteDC(hDCOnLed);
			DeleteDC(hDCOffLed);

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
						"8-Bit LED Board - Port 0x40",
						WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT,
						240, 64,
						NULL, NULL,
						::hModule, NULL);
	ShowWindow(hWndMain, SW_SHOW);

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
	portVal.val=read_io_port(b, 0x40);
	return 0;
}

int VirtualDevice_Terminate()
{
	// Destroy window here
	SendMessage(hWndMain, WM_CLOSE, NULL, NULL);
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
