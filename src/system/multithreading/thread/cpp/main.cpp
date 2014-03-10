#include <iostream>
#include <conio.h>
#include "thread.h"

using namespace std;

DWORD WINAPI PrintLine(LPVOID lpParam){
    for(int i=0; i<20; i++){
        cout << "a";
    }
    cout << endl;
    return 0;
}

int main()
{
    Thread myThread(&PrintLine, 0);
    myThread.Start();
    _getch();
    return 0;
}
