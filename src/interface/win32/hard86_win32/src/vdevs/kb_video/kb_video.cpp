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
const int BUFFER_W	= 80;
const int BUFFER_H	= 25;

const int CUR_BLINK_RATE = 15;	// toggle every n frames
const int CUR_W = CHAR_W;
const int CUR_H = 4;

int cur_x=0, cur_y=0;

queue<char> textBuffer;

/*
	HEX    BIN        COLOR

	0      0000      black
	1      0001      blue
	2      0010      green
	3      0011      cyan
	4      0100      red
	5      0101      magenta
	6      0110      brown
	7      0111      light gray
	8      1000      dark gray
	9      1001      light blue
	A      1010      light green
	B      1011      light cyan
	C      1100      light red
	D      1101      light magenta
	E      1110      yellow
	F      1111      white
*/

COLORREF colorTable16[16]={
	RGB(0, 0, 0),
	RGB(0, 0, 255),
	RGB(0, 255, 0),
	RGB(0, 255, 255),
	RGB(255, 0, 0),
	RGB(255, 0, 255),
	RGB(128, 64, 64),
	RGB(192, 192, 192),
	RGB(128, 128, 128),
	RGB(0, 128, 192),
	RGB(128, 255, 128),
	RGB(128, 255, 255),
	RGB(255, 128, 128),
	RGB(255, 128, 255),
	RGB(255, 255, 0),
	RGB(255, 255, 255)
};

bool screenUpdateReq=false;
char screenBuffer[2][BUFFER_H][BUFFER_W];	// plane 0 is attrib, plane 1 is data

bool bExiting=false;

sys_state_ptr sysState=NULL;

void DrawChar(HDC hDC, int x, int y, char c, uint8 attrib){
	char s[2]={c, 0};
	if(!c) return;
	COLORREF oldTextColor=SetTextColor(hDC, colorTable16[attrib & 0xf]);
	COLORREF oldBkColor=SetBkColor(hDC, colorTable16[attrib >> 4]);
	TextOutA(hDC, x, y, s, 1);
	SetTextColor(hDC, oldTextColor);
	SetTextColor(hDC, oldBkColor);
}

void DrawCursor(HDC hDC, int x, int y){
	static int count=0;
	static bool is_visible=false;
	if( (++count) % CUR_BLINK_RATE){
		count=0;
		is_visible=!is_visible;
	}
	if(is_visible){
		COLORREF oldTextColor=SetTextColor(hDC, RGB(255, 255, 255));
		COLORREF oldBkColor=SetBkColor(hDC, RGB(0, 0, 0));
		Rectangle(hDC, x, y+(CHAR_H-CUR_H), x+CHAR_W, y+CHAR_H);
		SetTextColor(hDC, oldTextColor);
		SetTextColor(hDC, oldBkColor);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	switch(uMsg){
	case WM_CREATE:
		SetTimer(hWnd, 1, (1000/30), NULL);
		break;
	case WM_TIMER:
		screenUpdateReq=true;
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_CHAR:
		textBuffer.push((char)wParam);
		SetEvent(hHasTextEvent);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		{
			hDC=BeginPaint(hWnd, &ps);
			HDC hMemDC=CreateCompatibleDC(hDC);
			HBITMAP hBmp=CreateCompatibleBitmap(hDC, SCREEN_W, SCREEN_H);
			HGDIOBJ hOldBmp=SelectObject(hMemDC, hBmp);
			HGDIOBJ hOldFont=SelectObject(hMemDC, (HGDIOBJ)GetStockObject(OEM_FIXED_FONT));
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			SetBkColor(hDC, RGB(0, 0, 0));
			SetTextColor(hDC, RGB(255, 255, 255));

			for(int i=0; i<BUFFER_H; i++){
				for(int j=0; j<BUFFER_W; j++){
					DrawChar(hMemDC, j*CHAR_W, i*CHAR_H, screenBuffer[1][i][j], screenBuffer[0][i][j]);
				}
			}

			DrawCursor(hMemDC, cur_x*CHAR_W, cur_y*CHAR_H);

			BitBlt(hDC, 0, 0, SCREEN_W, SCREEN_H, hMemDC, 0, 0, SRCCOPY);

			SelectObject(hMemDC, hOldBmp);
			DeleteObject(hOldBmp);
			DeleteDC(hMemDC);

			SelectObject(hDC, hOldFont);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_CLOSE:
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
			WaitForSingleObject((HANDLE)::sysState->sys_mutex, INFINITE);
			write_io_port(::sysState, (uint8)textBuffer.front(), 0x60);
			textBuffer.pop();
			make_int_call(::sysState, 0x7);
			if(textBuffer.empty()){
				ResetEvent(hHasTextEvent);
			}
			ReleaseMutex((HANDLE)::sysState->sys_mutex);
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
						L"Hard86 Keyboard/Screen Device",
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
	if(::screenUpdateReq){
		uint32 addr=0xb800 << 4;
		for(int i=0; i<BUFFER_H; i++){
			for(int j=0; j<BUFFER_W; j++){
				::screenBuffer[0][i][j]=read_mem_8(b, addr+(i*BUFFER_W*2)+(j*2));
				::screenBuffer[1][i][j]=read_mem_8(b, addr+(i*BUFFER_W*2)+(j*2)+1);
			}
		}
		::screenUpdateReq=false;
		
		addr=0x40 << 4;
		uint16 cur_offs=read_mem_16(b, addr);
		cur_x = read_mem_8(b, addr + cur_offs);
		cur_offs=read_mem_16(b, addr+2);
		cur_y = read_mem_8(b, addr + cur_offs);
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

int VirtualDevice_Initialize(void* a, void* b)
{
	hMainThread=CreateThread(NULL, NULL, MainThread, NULL, NULL, NULL);
#ifdef BUILD_EXE
	WaitForSingleObject(hMainThread, INFINITE);
#endif
	return 0;
}
