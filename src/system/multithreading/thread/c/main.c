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
    thread_create(&PrintStuff, NULL, &thread);
    thread_start(&thread);

    _getch();
    thread_close(&thread);
    return 0;
}
