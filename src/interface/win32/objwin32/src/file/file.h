#ifndef FILE_H
#define FILE_H

#include <Windows.h>
#include <string>
#include <list>

using namespace std;

namespace nsObjWin32{

namespace nsFiles{

	/** 
	 * FileObject base class
	 * Defines the basic behaviour of File objects (disk files, directories)
	 */
	class __declspec(dllexport) FileObject
	{
	public:
		FileObject(){ m_bOpened=false; m_hFile=NULL; }
		virtual ~FileObject(){}

		virtual bool Open()=0;
		virtual bool Close()=0;
		virtual bool Delete()=0;
		virtual bool Create()=0;
		virtual bool Move(const wchar_t* destPath);
		virtual bool Move(const wstring& destPath);
		virtual bool Copy(const wchar_t* destPath)=0;
		virtual bool Copy(const wstring& destPath)=0;
		virtual bool Exists();
		virtual bool SetPath(const wchar_t* path){ m_path=path; return 0; }
		virtual bool SetPath(const wstring& path){return SetPath(path.c_str());}
		const wchar_t* GetPath(){return m_path.c_str();}

		static wstring GetRelativePath(const wstring& absPath, const wstring& relativeTo);
		static wstring GetAbsolutePath(const wstring& relPath, const wstring& relativeTo);
		static wstring GetDirectory(const wstring& path);
	protected:
		wstring m_path;
		bool m_bOpened;

		HANDLE m_hFile;

		DWORD dwDesiredAccess;
		DWORD dwShareMode;
		DWORD dwFlagsAndAttributes;

		static wstring& TidySlashes(wstring& str, wchar_t wantedSlash=L'\\');

		// helper functions for GetRelativePath()
		// Case insensitive character comparison
		static int GetLastSimilarDirIndex(const wstring& path1, const wstring& path2);
		static void WStrToLower(wstring& str);
		static int GetDirLevels(const wstring& path);

		// helper functions for GetAbsolutePath()
		static void CutDirectory(wstring& str);
		static wstring GetNextPathChunk(const wstring& str);
	};

	/**
	 * File class
	 */
	class __declspec(dllexport) File  : public FileObject
	{
	protected:
		void Init(const wchar_t* path){
			SetPath(path);

			dwDesiredAccess=GENERIC_READ | GENERIC_WRITE;
			dwShareMode=0;
			dwFlagsAndAttributes=FILE_ATTRIBUTE_NORMAL;
		}
	public:
		File(){
			m_size=0;
			m_fileExt=L"";

			dwDesiredAccess=GENERIC_READ | GENERIC_WRITE;
			dwShareMode=0;
			dwFlagsAndAttributes=FILE_ATTRIBUTE_NORMAL;
		}

		File(const wchar_t* path){
			m_size=0;
			Init(path);
		}

		File(const wstring& path){
			m_size=0;
			Init(path.c_str());
		}

		virtual ~File(){
			Close();
		}

		bool Open();
		bool Create();
		bool Close();
		int Size();

		bool SetPath(const wchar_t* path);
		bool SetPath(const wstring& path);

		const wstring& GetExtension(){return m_fileExt;}

		static wstring GetExt(const wstring& path);

		bool Delete();
		bool Copy(const wchar_t* destPath);
		bool Copy(const wstring& destPath);

		// Data access
		int Write(void* data, int elemCount, int elemSize=1);
		int Write(wchar_t* str);
		int Write(char* str);

		int Read(void* buffer, int elemCount, int elemSize=1);

		bool SetPointer(LONG newFp, DWORD moveMethod=FILE_BEGIN);
		int GetPointer();
		bool SetEOF();
	protected:
		int m_size;
		wstring m_fileExt;
	};

	/**
	 * Directory class
	 */
	class __declspec(dllexport) Directory : public FileObject
	{
	protected:
		void Init(const wchar_t* path){
			SetPath(path);
		}
	public:
		Directory(){
			Init(L"");
		}

		Directory(const wchar_t* path){
			Init(path);
		}

		Directory(const wstring& path){
			Init(path.c_str());
		}

		virtual ~Directory(){

		}

		bool List(list<File>& fileList);

		bool Open();
		bool Close();
		bool Delete();
		bool Create();
		bool Move(const wchar_t* destPath);
		bool Move(const wstring& destPath);
		bool Copy(const wchar_t* destPath);
		bool Copy(const wstring& destPath);
	protected:
		bool IsSpecialDir(const wchar_t name[]);

		void Delete_Rec(const wstring& dir);
		void List_Rec(const wstring& dir, list<File>& fileList);
	};
}

}
#endif