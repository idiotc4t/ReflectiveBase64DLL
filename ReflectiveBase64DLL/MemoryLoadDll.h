#ifndef _MM_LOAD_DLL_H_
#define _MM_LOAD_DLL_H_


#include <Windows.h>


typedef BOOL(__stdcall* typedef_DllMain)(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved);


LPVOID MmLoadLibrary(LPVOID lpData, DWORD dwSize);

DWORD GetSizeOfImage(LPVOID lpData);

BOOL MmMapFile(LPVOID lpData, LPVOID lpBaseAddress);

DWORD Align(DWORD dwSize, DWORD dwAlignment);

BOOL DoRelocationTable(LPVOID lpBaseAddress);

BOOL DoImportTable(LPVOID lpBaseAddress);

BOOL SetImageBase(LPVOID lpBaseAddress);

BOOL CallDllMain(LPVOID lpBaseAddress);

LPVOID MmGetProcAddress(LPVOID lpBaseAddress, PCHAR lpszFuncName);

BOOL MmFreeLibrary(LPVOID lpBaseAddress);


#endif