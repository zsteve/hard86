#include <Windows.h>
#include <cstdio>
#include <iostream>

#include "mutex.h"

using namespace std;

int main(){
	Mutex mutex;
	cout << mutex.Lock() << endl;
	cout << mutex.Unlock() << endl;
	cout << mutex.Unlock() << endl;
	cout << mutex.Lock() << endl;
    return 0;
}
