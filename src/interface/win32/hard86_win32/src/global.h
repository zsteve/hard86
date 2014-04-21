#ifndef HARD86_WIN32_GLOBAL_H
#define HARD86_WIN32_GLOBAL_H

namespace nsHard86Win32{

	enum AppWindows{ WND_MAINFRAME };

	const int DEF_BUTTON_W=80;
	const int DEF_BUTTON_H=24;
	const int DEF_SCROLLBAR_W=16;

	void WriteDebug(const wchar_t* message, const wchar_t* func, const wchar_t* file, int line);

}

#endif