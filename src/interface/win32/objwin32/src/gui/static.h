#ifndef STATIC_H
#define STATIC_H

#include "window.h"
#include "common_control.h"

namespace nsObjWin32{

namespace nsGUI{

	class DLLEXPORT Static : public CommonControl
	{
	public:

		Static(){
			m_className=L"STATIC";
		}
		virtual ~Static(){}

	protected:
		HWND Create(DWORD dwExStyle,
				LPCTSTR lpWindowName,
				DWORD dwStyle,
				int x, int y,
				int w, int h,
				HWND hwndParent,
				HMENU hMenu);
	};

	class DLLEXPORT TextStatic : public Static
	{
	public:
		TextStatic(DWORD style=0, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}
		virtual ~TextStatic(){}

		HWND Create(LPCTSTR lpWindowName, int x, int y, int w, int h, HWND hwndParent, int ID);

	private:

	};
	
	/**
	 * Bitmap static control
	 */
	class DLLEXPORT BmpStatic : public Static
	{
	public:

		BmpStatic(DWORD style=SS_BITMAP, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~BmpStatic(){}

		/** 
		 * @param lpBmpResId - Resource identifier of the bitmap to use
		 */
		HWND Create(LPCTSTR lpBmpResId, int x, int y, int w, int h, HWND hwndParent, int ID);
	};

	class DLLEXPORT FrameStatic : public Static
	{
	public:

		FrameStatic(DWORD style=SS_GRAYFRAME, DWORD exStyle=0){
			m_style|=style;
			m_exStyle|=exStyle;
		}

		virtual ~FrameStatic(){}

		HWND Create(LPCTSTR lpWindowName, int x, int y, int w, int h, HWND hwndParent, int ID);
	};

}

}

#endif