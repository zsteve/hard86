#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "mutex.h"

#include "../../thread/c/thread.h"

DWORD WINAPI PrintLine1(LPVOID param)
{
    MUTEX mutex=(MUTEX)param;
    int i=0;
    int j=0;
    while(j++<20){
        Mutex_Lock(mutex);
        for(i=0; i<20; i++){
            putchar('b');
        }
        putchar('\n');
        if(!Mutex_Unlock(mutex)){
            printf("Error\n");
        }
    }
    return 0;
}

DWORD WINAPI PrintLine2(LPVOID param)
{
    MUTEX mutex=(MUTEX)param;
    int i=0;
    int j=0;
    while(j++<20){
        Mutex_Lock(mutex);
        for(i=0; i<20; i++){
            putchar('a');
        }
        putchar('\n');
        if(!Mutex_Unlock(mutex)){
            printf("Error\n");
        }
    }
    return 0;
}

int main()
{
    MUTEX myMutex;
    THREAD myThread1, myThread2;
    myMutex=Mutex_Create();
    Thread_Create(&PrintLine1, (LPVOID)myMutex, &myThread1);
    Thread_Create(&PrintLine2, (LPVOID)myMutex, &myThread2);
    Thread_Start(&myThread1);
    Thread_Start(&myThread2);
    _getch();
    Mutex_Close(myMutex);
    return 0;
}
