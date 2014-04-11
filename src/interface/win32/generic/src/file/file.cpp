#include "file.h"
#include "../global.h"

namespace nsObjWin32{

namespace nsFiles{

	using namespace nsGlobal;

	// FileObject
	bool FileObject::Move(const wchar_t* destPath){
		bool wasOpened=m_bOpened;
		if(m_bOpened)
			Close();
		BOOL retv=MoveFile(m_path.c_str(), destPath);
		if(retv){
			SetPath(destPath);
		}
		if(retv && wasOpened){
			Open();
		}
		return retv;
	}

	bool FileObject::Move(const wstring& destPath){
		return Move(destPath.c_str());
	}

	bool FileObject::Exists(){
		if(m_bOpened){
			Close();
			bool retv=Open();
			return retv;
		}
		bool retv=Open();
		Close();
		return retv;
	}

	// File
	/**
	 * Opens current file
	 * @return true = success, false = failure
	 */
	bool File::Open(){
		if(m_bOpened) return false;
		m_hFile=CreateFile(m_path.c_str(), dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL); 
		return (m_bOpened=m_hFile ? true : false);
	}

	/**
	 * Creates and opens new file
	 * @return true = success, false = failure
	 */
	bool File::Create(){
		if(m_bOpened){
			Close();
		}
		m_hFile=CreateFile(m_path.c_str(), dwDesiredAccess, dwShareMode, NULL, CREATE_NEW, dwFlagsAndAttributes, NULL); 
		return (m_bOpened=m_hFile ? true : false);
	}

	/**
	 * Closes current file
	 */
	bool File::Close(){
		CloseHandle(m_hFile);
		m_hFile=NULL;
		m_bOpened=false;
		return 0;
	}

	/**
	 * Returns file size
	 */
	int File::Size(){
		if(!m_bOpened){
			Open();
			int size=GetFileSize(m_hFile, NULL);
			Close();
			return size;
		}
		return GetFileSize(m_hFile, NULL);
	}

	/**
	 * Sets path
	 */
	bool File::SetPath(const wchar_t* path){
		bool wasOpened=m_bOpened;
		if(m_bOpened){
			Close();
		}
		FileObject::SetPath(path);
		m_fileExt=GetExt(path);
		if(wasOpened){
			Open();
		}
		return 0;
	}

	bool File::SetPath(const wstring& path){
		return SetPath(path.c_str());
	}

	wstring File::GetExt(const wstring& path){
		wstring::const_iterator it=path.end();
		while(--it>=path.begin() && (*it)!=L'\\' && (*it)!=L'/'){
			if(*it==L'.'){
				return path.substr(it-path.begin()+1, wstring::npos);
			}
		}
	}

	bool File::Delete(){
		Close();
		return DeleteFile(m_path.c_str());
	}

	bool File::Copy(const wchar_t* destPath){
		bool wasOpened=m_bOpened;
		if(m_bOpened){
			Close();
		}
		BOOL retv=CopyFile(m_path.c_str(), destPath, true);
		if(wasOpened){
			Open();
		}
		return retv;
	}

	bool File::Copy(const wstring& destPath){
		return Copy(destPath.c_str());
	}

	/** 
	 * Writes to file
	 * @return -1 for failure, any other value represents 'bytes written'
	 */
	int File::Write(void* data, int elemCount, int elemSize){
		if(!m_bOpened) return -1;
		DWORD bytesWritten;
		bool retv=WriteFile(m_hFile, data, (elemCount*elemSize), &bytesWritten, NULL);
		if(!retv) return -1;
		return bytesWritten;
	}

	int File::Write(wchar_t* str){
		return Write(str, wcslen(str)+1, sizeof(wchar_t));
	}

	int File::Write(char* str){
		return Write(str, strlen(str)+1, sizeof(char));
	}

	/** 
	 * Reads from file
	 * @return -1 for failure, any other value represents bytes read
	 */
	int File::Read(void* buffer, int elemCount, int elemSize){
		if(!m_bOpened) return -1;
		DWORD bytesRead;
		if(!ReadFile(m_hFile, buffer, (elemCount*elemSize), &bytesRead, NULL)){
			return -1;
		}
		return bytesRead;
	}

	/**
	 * Sets file pointer value
	 * @return -1 for failure, otherwise returns new pointer
	 */
	bool File::SetPointer(LONG newFp, DWORD moveMethod){
		int retv=SetFilePointer(m_hFile, newFp, NULL, moveMethod);
		return (retv==INVALID_SET_FILE_POINTER) ? false : true;
	}

	/**
	 * Gets file pointer value
	 */
	int File::GetPointer(){
		return SetFilePointer(m_hFile, 0, 0, FILE_CURRENT);
	}

	/**
	 * Sets end of file to the current file pointer
	 */
	bool File::SetEOF(){
		return SetEndOfFile(m_hFile);
	}
}

}