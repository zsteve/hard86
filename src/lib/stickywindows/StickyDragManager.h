#ifndef _STICKYDRAGMANAGER_H
#define _STICKYDRAGMANAGER_H

#include <windows.h>
#include <vector>
using namespace std;

class CStickyDragManager
{
public:
	CStickyDragManager(bool isDocker = false);
	virtual ~CStickyDragManager();
	void Init(HWND hWnd = 0);
	vector<HWND> *SnapMove(LPRECT activeRect, vector<RECT>& dockedRects);
	void SnapMoveRect(LPRECT r);
	void SnapSize(LPRECT r);
	void GetDockedWindows(HWND hWnd, int index, vector<HWND> &windows);
	void StartTrack();

	inline void SetDockerState(bool bDockerState) {	m_bIsDocker = bDockerState;	}
	inline void AddWindow(HWND hWnd) { m_registeredWindows.push_back(hWnd); }
	inline void AddDesktopWindow() { HWND hWnd = ::GetDesktopWindow(); AddWindow(hWnd); m_exclusionWindow = hWnd; }
	inline HWND GetActiveWindow() { return m_activeWindow; }
	inline bool IsCloseTo(int ms, int ss) {	return ((ms > (ss - m_slack)) && (ms < (ss + m_slack))) ? true : false;	}
	inline vector<HWND>* DockedWindows() {	return &m_dockedWindows; }
	inline vector<HWND>* UnDockedWindows() {	return &m_undockedWindows; }
	inline bool IsDocker() { return m_bIsDocker; }
protected:
	RECT   m_lastDragRect;
	bool   m_bIsDocker;
	POINT  m_dragPos;
	int    m_slack;
	bool   m_stickFlags[4];
	int    m_stickPos[4];
	HWND   m_activeWindow;
	HWND   m_exclusionWindow;
	vector<HWND> m_undockedWindows;
	vector<HWND> m_registeredWindows;
	vector<HWND> m_dockedWindows;
	vector<bool> m_windowDock;
	vector<bool> m_windowDockProcess;
};

#endif //_STICKYDRAGMANAGER_H
