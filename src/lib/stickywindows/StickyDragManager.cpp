// Sticky Drag Manager
// TODO : Find source and acknowledge

#include "StickyDragManager.h"



CStickyDragManager::CStickyDragManager(bool isDocker)
{
	Init();
	m_bIsDocker = isDocker;
}

CStickyDragManager::~CStickyDragManager()
{
}

void CStickyDragManager::Init(HWND hWnd)
{
	m_activeWindow = hWnd;
	m_dockedWindows.clear();
	m_undockedWindows.clear();
	m_registeredWindows.clear();
	m_slack = 5; // slack determines how strongly attracted registered windows are to each other.
	m_exclusionWindow = 0;
	for (int i = 0; i < 4; i++)
	{
		m_stickFlags[i] = false;
		m_stickPos[i]   = 0;
	}
}
	

vector<HWND> *CStickyDragManager::SnapMove(LPRECT activeRect, vector<RECT>& dockedRects)
{
	int dx = 0, dy = 0;
	RECT ssar, sar, sr;
	dockedRects.clear();
	ssar = *activeRect;
	int dxy[2];
	dxy[0] = activeRect->left - m_lastDragRect.left;
	dxy[1] = activeRect->top  - m_lastDragRect.top;
	if (!m_dockedWindows.size() || !m_bIsDocker)
	{
		SnapMoveRect(activeRect);
		m_lastDragRect = *activeRect;
		return &m_dockedWindows;
	}
	for (unsigned int i = 0; i < m_dockedWindows.size(); i++)
	{
		RECT r;
		GetWindowRect(m_dockedWindows[i], &r);
		OffsetRect(&r, dxy[0], dxy[1]);
		dockedRects.push_back(r);
	}
	// Perturb windows until their positions stabilize (give up after 10 iterations).
	int safetyCount = 0;
	do
	{
		sar = *activeRect;
		SnapMoveRect(activeRect);
		dx = sar.left - activeRect->left;
		dy = sar.top - activeRect->top;
		for (unsigned i = 0; i < m_dockedWindows.size(); i++)
		{
			OffsetRect(&dockedRects[i], -dx, -dy);
			sr = dockedRects[i];
			SnapMoveRect(&dockedRects[i]);
			dx = sr.left - dockedRects[i].left;
			dy = sr.top - dockedRects[i].top;
			for (unsigned int j = 0; j < m_dockedWindows.size(); j++)
				if (j != i)
					OffsetRect(&dockedRects[j], -dx, -dy);
			OffsetRect(activeRect, -dx, -dy);
		}
		dx = sar.left - activeRect->left;
		dy = sar.top - activeRect->top;
	}
	while (dx || dy && (++safetyCount < 10));
	dx = ssar.left - activeRect->left;
	dy = ssar.top - activeRect->top;
	for (int i = 0; i < m_dockedWindows.size(); i++)
	{
		GetWindowRect(m_dockedWindows[i], &dockedRects[i]);
		OffsetRect(&dockedRects[i], dxy[0]-dx, dxy[1]-dy);
	}
	m_lastDragRect = *activeRect;
	return &m_dockedWindows;
}

void CStickyDragManager::SnapMoveRect(LPRECT r)
{
	POINT cPos;
	GetCursorPos(&cPos);
	int mPos[4] = {cPos.x, cPos.y, cPos.x, cPos.y};
	for (int j = 0; j < 4; j++)
	{
		if (m_stickFlags[j])
		{
			if (abs(m_stickPos[j] - mPos[j]) > 2 * m_slack)
			{
				m_stickFlags[j] = false;
				int d = mPos[j] - m_stickPos[j];
				switch(j)
				{
					case 0:	OffsetRect(r, d, 0); break;
					case 1:	OffsetRect(r, 0, d); break;
					case 2:	OffsetRect(r, d, 0); break;
					case 3:	OffsetRect(r, 0, d); break;
				}
			}
		}
	}
	int dx = 0;
	int dy = 0;
	int iPos = -1;
	int jPos = -1;
	RECT cr, dr, sr;
	for (unsigned int i = 0; i < m_undockedWindows.size(); i++)
	{
		GetWindowRect(m_undockedWindows[i], &cr);
		sr = cr;
		InflateRect(&sr, m_slack, m_slack);
		if (!IntersectRect(&dr, &sr, r))
			continue;
		bool b1 = false;
		if ((b1 = IsCloseTo(r->right, cr.left)) || (IsCloseTo(r->right, cr.right)))
		{
			dx = r->right - (b1 ? cr.left : cr.right);	iPos = 2;
		}
		else if ((b1 = IsCloseTo(r->left, cr.left)) || (IsCloseTo(r->left, cr.right)))
		{
			dx = r->left - (b1 ? cr.left : cr.right);	iPos = 0;
		}
		if ((b1 = IsCloseTo(r->top, cr.top)) || (IsCloseTo(r->top, cr.bottom)))
		{
			dy = r->top - (b1 ? cr.top : cr.bottom); jPos = 1;
		}
		else if ((b1 = IsCloseTo(r->bottom, cr.top)) || (IsCloseTo(r->bottom, cr.bottom)))
		{
			dy = r->bottom - (b1 ? cr.top : cr.bottom); jPos = 3;
		}
		OffsetRect(r, -dx, -dy);
		if (iPos > -1 && !m_stickFlags[iPos])
		{
			m_stickFlags[iPos] = true;
			m_stickPos[iPos] = mPos[iPos];
		}
		if (jPos > -1 && !m_stickFlags[jPos])
		{
			m_stickFlags[jPos] = true;
			m_stickPos[jPos] = mPos[jPos];
		}
		if (dx || dy)
			break;
	}
}

void CStickyDragManager::SnapSize(LPRECT r)
{
	RECT cr, dr, sr;
	for (unsigned int i = 0; i < m_undockedWindows.size(); i++)
	{
		GetWindowRect(m_undockedWindows[i], &cr);
		sr = cr;
		InflateRect(&sr, m_slack, m_slack);
		if (!IntersectRect(&dr, &sr, r))
			continue;
		bool b1 = false;
		if ((b1 = IsCloseTo(r->right, cr.left)) || (IsCloseTo(r->right, cr.right)))
			r->right -= r->right - (b1 ? cr.left : cr.right);
		if ((b1 = IsCloseTo(r->left, cr.left)) || (IsCloseTo(r->left, cr.right)))
			r->left -= r->left - (b1 ? cr.left : cr.right);
		if ((b1 = IsCloseTo(r->top, cr.top)) || (IsCloseTo(r->top, cr.bottom)))
			r->top -= r->top - (b1 ? cr.top : cr.bottom);
		if ((b1 = IsCloseTo(r->bottom, cr.top)) || (IsCloseTo(r->bottom, cr.bottom)))
			r->bottom -= r->bottom - (b1 ? cr.top : cr.bottom);
	}
}

void CStickyDragManager::GetDockedWindows(HWND hWnd, int index, vector<HWND> &windows)
{
	RECT wr, twr, dr;
	if (index == -1)
	{
		m_windowDock.clear();
		m_windowDockProcess.clear();
		for (unsigned int i = 0; i < m_registeredWindows.size(); i++)
		{
			m_windowDock.push_back(false);
			m_windowDockProcess.push_back(false);
		}
	}
	else
		m_windowDockProcess[index] = true;
	GetWindowRect((index == -1) ? hWnd : m_registeredWindows[index], &wr);
	for (unsigned int i = 0; i < m_registeredWindows.size(); i++)
	{
		if (m_exclusionWindow == m_registeredWindows[i])
			continue;
		if (i != index && !m_windowDock[i])
		{
			GetWindowRect(m_registeredWindows[i], &twr);
			RECT cwr = wr;
			InflateRect(&cwr, 1, 1);
			if (!IntersectRect(&dr, &twr, &cwr))
				continue;
			if ((twr.left == wr.left)  || (twr.left == wr.right))    m_windowDock[i] = true;
			if ((twr.right == wr.left) || (twr.right == wr.right))   m_windowDock[i] = true;
			if ((twr.top == wr.top)    || (twr.top == wr.bottom))    m_windowDock[i] = true;
			if ((twr.bottom == wr.top) || (twr.bottom == wr.bottom)) m_windowDock[i] = true;
			if (!m_windowDockProcess[i] && m_windowDock[i])
				GetDockedWindows(m_registeredWindows[i], i, windows);
		}
	}
	if (index == -1)
	{
		windows.clear();
		for (unsigned int i = 0; i < m_registeredWindows.size(); i++)
			if (m_windowDock[i])
				windows.push_back(m_registeredWindows[i]);
	}
}

void CStickyDragManager::StartTrack()
{
	for (int i = 0; i < 4; i++)
	{
		m_stickFlags[i] = false;
		m_stickPos[i]   = 0;
	}
	m_undockedWindows.clear();
	m_dockedWindows.clear();
	if (m_bIsDocker)
	{
		if (m_activeWindow)
			GetDockedWindows(m_activeWindow, -1, m_dockedWindows);
		// Exclude docked windows from dock candidates.
		for (unsigned int i = 0; i < m_registeredWindows.size(); i++)
		{
			bool docked = false;
			for (unsigned int j = 0; j < m_dockedWindows.size(); j++)
				if (m_registeredWindows[i] == m_dockedWindows[j]) docked = true;
			if (!docked)
				m_undockedWindows.push_back(m_registeredWindows[i]);
		}
	}
	else
	{
		m_undockedWindows = m_registeredWindows;
	}
	GetWindowRect(m_activeWindow, &m_lastDragRect);
}


