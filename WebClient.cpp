#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>
using namespace std;
#define DEFAULT_PORT "80"
#define DEFAULT_BUFLEN 512

int main(int argc, char** argv) {
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(wVersion, &wsaData);
	if (argc != 2) {
		printf("Usage: WebClient.exe www.sjtu.edu.cn\n");
		return 1;
	}
	if (iResult != 0)
	{
		printf("WSAStartup Failed: %d\n", iResult);
		return 1;
	}
	struct addrinfo *result = NULL,
		 *ptr = NULL,
		 hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("addrinfo failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKET Client = INVALID_SOCKET;
	ptr = result;
	Client = socket(ptr->ai_family,ptr->ai_socktype, ptr->ai_protocol);
	if (Client == INVALID_SOCKET) {
		printf("soceket create failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	iResult = connect(Client, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(Client);
		Client = INVALID_SOCKET;
		return 1;
	}
	freeaddrinfo(result);
	if (Client == INVALID_SOCKET) {
		printf("connect failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}
	
	int recvlen = DEFAULT_BUFLEN;
	char recvf[DEFAULT_BUFLEN];
	const char* sendbuf = "GET / HTTP/1.0\n\n";
	iResult = send(Client, sendbuf, (int)sizeof(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send data failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("Bytes sent: %d\n", iResult);
	iResult = shutdown(Client, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", iResult);
		closesocket(Client);
		WSACleanup();
		return 1;
	}
	do {
		iResult = recv(Client, recvf, recvlen, 0);
		if (iResult > 0) {
			printf("Bytes recvived: %s\n", recvf);
		}
		else if (iResult == 0) {
			printf("Connection shutdown.\n");
		}
		else {
			printf("recvive failed: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	closesocket(Client);
	WSACleanup();
	return 0;
}