#include "main.h"

// a sample exported function

using namespace std;

void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

int DLL_EXPORT VirtualDevice_Initialize(void* a, void* b){

	return 0;
}

int DLL_EXPORT VirtualDevice_Terminate(){

	return 0;
}

int DLL_EXPORT VirtualDevice_AcceptEmulationMutex(MUTEX a, sys_state_ptr b){
	/*FILE* f;
	f=fopen("vdev_out.txt", "a");
	fprintf(f, "VDevTest callback : SP : %X, IP : %X\n", b->sp, b->ip);
	fclose(f);*/
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // succesful
}
