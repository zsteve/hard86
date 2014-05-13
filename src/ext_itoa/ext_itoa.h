#ifndef EXT_ITOA_H
#define EXT_ITOA_H

#include <cstdlib>
#include <cstring>
#include <string>

using namespace std;

void ext_itoa(int value, char* str, int radix, int padto=0, char padchar='0'){
	char buf[10]="";
	itoa(value, buf, radix);
	if(padto){
		int padspaces=(padto-strlen(buf));
		int i=padspaces;
		if(i > 0){
			while(i){
				str[--i]=padchar;
			}
			str[padspaces]=NULL;
			strcat(str, buf);
			return;
		}
	}
	strcpy(str, buf);
}

void ext_itow(int value, wchar_t* str, int radix, int padto=0, wchar_t padchar=L'0'){
	wchar_t buf[10]=L"";
	_itow(value, buf, radix);
	if(padto){
		int padspaces=(padto-wcslen(buf));
		int i=padspaces;
		if(i > 0){
			while(i){
				str[--i]=padchar;
			}
			str[padspaces]=NULL;
			wcscat(str, buf);
			return;
		}
	}
	wcscat(str, buf);
}

wstring ext_itow(int value, int radix, int padto=0, wchar_t padchar=L'0'){
	wchar_t* buf=new wchar_t[padto+10];
	buf[0]=NULL;
	ext_itow(value, buf, radix, padto, padchar);
	wstring retv(buf);
	delete[] buf;
	return retv;
}

string ext_itoa(int value, int radix, int padto=0, char padchar=L'0'){
	char* buf=new char[padto+10];
	buf[0]=NULL;
	ext_itoa(value, buf, radix, padto, padchar);
	string retv(buf);
	delete[] buf;
	return retv;
}

#endif
