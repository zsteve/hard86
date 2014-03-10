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
        mutex_lock(mutex);
        for(i=0; i<20; i++){
            putchar('b');
        }
        putchar('\n');
        if(!mutex_unlock(mutex)){
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
        mutex_lock(mutex);
        for(i=0; i<20; i++){
            putchar('z');
        }
        putchar('\n');
        if(!mutex_unlock(mutex)){
            printf("Error\n");
        }
    }
    return 0;
}

int main()
{
    MUTEX myMutex;
    THREAD myThread1, myThread2;
    myMutex=mutex_create();
    thread_create(&PrintLine1, (LPVOID)myMutex, &myThread1);
    thread_create(&PrintLine2, (LPVOID)myMutex, &myThread2);
    thread_start(&myThread1);
    thread_start(&myThread2);
    _getch();
    mutex_close(myMutex);
    return 0;
}
