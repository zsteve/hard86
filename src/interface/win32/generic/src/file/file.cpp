#include <cassert>

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
	
	wstring FileObject::GetRelativePath(const wstring& absPath, const wstring& relativeTo){
		wstring relPath(L"");
		wstring tmpPath=absPath;
		wstring basePath=relativeTo;
		WStrToLower(tmpPath);
		WStrToLower(basePath);

		int lastSimilar=GetLastSimilarDirIndex(tmpPath, basePath);
		int dirLevelsUp=GetDirLevels(basePath.substr(lastSimilar));
		for(int i=0; i<dirLevelsUp-1; i++){
			relPath+=L"\\..";
		}
		relPath+=tmpPath.substr(lastSimilar);
		TidySlashes(relPath);
		return relPath;
	}

	int FileObject::GetLastSimilarDirIndex(const wstring& path1, const wstring& path2){
		const wstring& shorter=(path1.size() > path2.size()) ? path2 : path1;
		const wstring& longer=(shorter==path1) ? path2 : path1;
		int j=0;
		for(int i=0; i<shorter.size(); i++){
			if(shorter[i]!=longer[i]) return j;
			if(shorter[i]==L'\\' || shorter[i]=='/') j=i;
		}
		return j;
	}

	int FileObject::GetDirLevels(const wstring& path){
		int levels=0;
		for(int i=0; i<path.size(); i++){
			if(path[i]==L'/' || path[i]==L'\\') levels++;
		}
		return levels;
	}

	void FileObject::WStrToLower(wstring& str){
		for(wstring::iterator it=str.begin();
			it!=str.end();
			++it){
			*it=towlower(*it);
		}
	}

	void FileObject::CutDirectory(wstring& str){
		for(int i=str.size()-1; i>=0; i--){
			if(str[i]==L'/' || str[i]==L'\\'){
				str=str.substr(0, i);
				return;
			}
		}
	}

	wstring FileObject::GetNextPathChunk(const wstring& str){
		for(int i=0; i<str.size(); i++){
			if(str[i]==L'/' || str[i]==L'\\'){
				return str.substr(0, i+1);
			}
		}
		return str;
	}

	/**
	 * Expects relPath to be in the form
	 * dir/file
	 * Expects relativeTo to be in the form
	 * dir/dir/ <- note '/' at end
	 */
	wstring FileObject::GetAbsolutePath(const wstring& relPath, const wstring& relativeTo){
		wstring tmpPath=relPath;
		wstring basePath=relativeTo;
		CutDirectory(basePath);
		int i=0;
		while(tmpPath.size()>0){
			wstring c=GetNextPathChunk(tmpPath);
			if(c==L"../" || c==L"..\\"){
				CutDirectory(basePath);
			}
			else{
				basePath+=L"/"+c;
			}
			tmpPath=tmpPath.substr(c.size());
		}
		return TidySlashes(basePath);

	}

	wstring& FileObject::TidySlashes(wstring& str, wchar_t wantedSlash){
		for(wstring::iterator it=str.begin();
			it!=str.end();
			++it){
			if(*it == L'\\' || *it == L'/') *it=wantedSlash;
		}
		return str;
	}

	// File
	/**
	 * Opens current file
	 * @return true = success, false = failure
	 */
	bool File::Open(){
		if(m_bOpened) return false;
		m_hFile=CreateFile(m_path.c_str(), dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL); 
		return (m_bOpened=m_hFile==INVALID_HANDLE_VALUE ? true : false);
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
		return (m_bOpened=m_hFile==INVALID_HANDLE_VALUE ? true : false);
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
		return wstring(L"");
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

	// Directory
	bool Directory::List(list<File>& fileList){
		List_Rec(m_path, fileList);
		return true;
	}

	bool Directory::IsSpecialDir(const wchar_t name[]){
		return (!(wcscmp(name, L".") &&
				wcscmp(name, L"..") &&
				wcscmp(name, L"...")));
	}

	void Directory::List_Rec(const wstring& dir, list<File>& fileList){
		WIN32_FIND_DATA fd;
		wstring fNewFile(dir+L"\\*");
		HANDLE hFind=FindFirstFile(fNewFile.c_str(), &fd);
		if(hFind==INVALID_HANDLE_VALUE){
			return;
		}
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			!IsSpecialDir(fd.cFileName)){
			wstring fNewDir(dir+L"\\"+fd.cFileName);
			List_Rec(fNewDir, fileList);
		}
		else if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
			File f(dir+L"\\"+fd.cFileName);
			fileList.push_back(f);
		}
		while(FindNextFile(hFind, &fd)){
			if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!IsSpecialDir(fd.cFileName)){
				wstring fNewDir(dir+L"\\"+fd.cFileName);
				List_Rec(fNewDir, fileList);
			}
			else if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				File f(dir+L"\\"+fd.cFileName);
				fileList.push_back(f);
			}
		}
	}

	bool Directory::Open()
	{
		if(m_bOpened){
			Close();
		}
		m_hFile=CreateFile(m_path.c_str(), dwDesiredAccess, dwShareMode, NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL);
		return (m_bOpened=m_hFile==INVALID_HANDLE_VALUE ? true : false);
	}

	bool Directory::Create(){
		if(m_bOpened){
			Close();
		}
		m_hFile=CreateFile(m_path.c_str(), dwDesiredAccess, dwShareMode, NULL, CREATE_NEW, dwFlagsAndAttributes, NULL);
		return (m_bOpened=m_hFile==INVALID_HANDLE_VALUE ? true : false);
	}

	bool Directory::Close(){
		CloseHandle(m_hFile);
		m_bOpened=false;
		return true;
	}

	/**
	 * Deletes the directory and all subdirectories
	 */
	bool Directory::Delete(){
		Close();
		Delete_Rec(m_path);
		return RemoveDirectory(m_path.c_str());
	}

	void Directory::Delete_Rec(const wstring& dir){
		WIN32_FIND_DATA fd;
		wstring fNewFile(dir+L"\\*");
		HANDLE hFind=FindFirstFile(fNewFile.c_str(), &fd);
		if(hFind==INVALID_HANDLE_VALUE){
			return;
		}
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			!IsSpecialDir(fd.cFileName)){
			wstring fNewDir(dir+L"\\"+fd.cFileName);
			Delete_Rec(fNewDir);
			RemoveDirectory(fNewDir.c_str());
		}
		else if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
			File f(dir+L"\\"+fd.cFileName);
			f.Delete();
		}
		while(FindNextFile(hFind, &fd)){
			if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
				!IsSpecialDir(fd.cFileName)){
				wstring fNewDir(dir+L"\\"+fd.cFileName);
				Delete_Rec(fNewDir);
				RemoveDirectory(fNewDir.c_str());
			}
			else if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
				File f(dir+L"\\"+fd.cFileName);
				f.Delete();
			}
		}
	}

	bool Directory::Move(const wchar_t* destPath){
		if(m_bOpened){
			Close();
		}
		bool retv=MoveFile(m_path.c_str(), destPath);
		if(retv){
			SetPath(destPath);
			if(m_bOpened){
				Open();
			}
		}
		return retv;
	}

	bool Directory::Move(const wstring& destPath){
		return Move(destPath.c_str());
	}

	bool Directory::Copy(const wchar_t* destPath){
		ALERT_ERR("Unimplemented - Directory::Copy");
		return 0;
	}

	bool Directory::Copy(const wstring& destPath){
		return Copy(destPath.c_str());
	}

}

}