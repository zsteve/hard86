#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#include "thread.h"

DWORD WINAPI PrintStuff(LPVOID param){
    printf("hello world!");
    return 0;
}

int main()
{
    THREAD thread;
    Thread_Create(&PrintStuff, NULL, &thread);
    Thread_Start(&thread);

    _getch();
    Thread_Close(&thread);
    return 0;
}
