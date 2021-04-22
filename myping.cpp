// myping.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<stdio.h>
#pragma comment(lib,"Ws2_32.lib")
#define TIME 4
#define BUFFER_LEN 512
#define REQUEST_TYPE byte(8)
#define ECHO_TYPE byte(0)
#define CODE byte(0)
#define PORT "300"
struct icmp_head {
	byte type;
	byte code;
	byte checksum[2];
};

unsigned short CheckSum(unsigned short*, int);
int handle_echo(char*, int);
int main(int argc, char** argv)
{
	WSADATA wsaData;
	int iResult;
	SOCKET icmp_request = INVALID_SOCKET;
	struct sockaddr_in remote;
	struct sockaddr_in fromaddr;
	int addrlen = sizeof(remote);
	int fromaddrlen = sizeof(fromaddr);
	char ipstringbuffer[46];
	char recvf[BUFFER_LEN];
	if (argc != 2) {
		printf("Usage: myping ip");
		return 1;
	}
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSA start up failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	//get ip
	struct addrinfo	*result,hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_family = AF_INET;
	iResult = getaddrinfo(argv[1], PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	memcpy(&remote, result->ai_addr, sizeof(remote));
	inet_ntop(AF_INET, &remote.sin_addr, ipstringbuffer, sizeof(ipstringbuffer));
	printf("ipget: %s\n", ipstringbuffer);
	//create socket
	icmp_request = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (icmp_request == INVALID_SOCKET) {
		printf("create socketfailed: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	//create icmp datagram
	byte icmp_pkg[512];
	struct icmp_head icmp_rq;
	memset(&icmp_rq, 0x00, sizeof(icmp_rq));
	icmp_rq.type = 0x08;
	icmp_rq.code = 0x00;
	byte flags[4];
	memset(flags, 0x00, sizeof(flags));
	char data[10] = "hello!";
	memset(icmp_pkg, 0x00, sizeof(icmp_pkg));
	memcpy(icmp_pkg, &icmp_rq, sizeof(icmp_rq));
	memcpy(icmp_pkg + sizeof(icmp_rq), flags, sizeof(flags));
	memcpy(icmp_pkg + sizeof(icmp_rq) + sizeof(flags), data, sizeof(data));
	int size = sizeof(icmp_rq) + sizeof(flags) + sizeof(data);
	*(unsigned short*)(((icmp_head*)icmp_pkg)->checksum)
		= CheckSum((unsigned short*)icmp_pkg, size);

	int count = 0;
	for (int i = 0;i < TIME;i++) {
		iResult = sendto(icmp_request, (char*)&icmp_pkg, size, 0, (struct sockaddr*)&remote, addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("send data error\n");
			break;
		}
		iResult = recvfrom(icmp_request, recvf, sizeof(recvf), 0, (struct sockaddr*)&fromaddr,(int*)&fromaddrlen);
		if (iResult > 0) {
			count += handle_echo(recvf+20, iResult-20);
		}
		else {
			printf("not receive data\n");
		}
	}

	printf("total send: %d, total receive:%d, drop rate: %d%", TIME, count, count * 100 / (double)TIME);
	return 0;
}

unsigned short CheckSum(unsigned short* ptr, int size) {
	unsigned short checksum = 0;
	while (size) {
		checksum += *ptr++;
		size -= sizeof(USHORT);
	}
	if (size) {
		checksum += *(UCHAR*)ptr;
	}
	checksum = (checksum >> 16) + (checksum & 0xFFFF);
	checksum += (checksum >> 16);
	return (USHORT)(~checksum);
}

int handle_echo(char* recvf, int len) {
	icmp_head header;
	byte* buf = (byte*) recvf;
	memcpy(&header, buf, sizeof(header));
	if (header.type == 0x00) {
		printf("receive normal echo: byte %d\n", len);
		return 1;
	}
	else if (header.type == 0x03) {
		switch (header.code) {
		case 0x00:printf("Target net can not be reached\n");break;
		case 0x01:printf("Target host can not be reached\n");break;
		case 0x02:printf("Target protocol can not be reached\n");break;
		case 0x03:printf("Target port can not be reached\n");break;
		case 0x04:printf("must be segment\n");break;
		case 0x05:printf("source route failed\n");break;
		case 0x06:printf("source route failed\n");break;
		case 0x07:printf("unknown target net\n");break;

		case 0x08:printf("unused\n");break;

		case 0x09:printf("\n");break;

		case 0x0a:printf("source route failed\n");break;




		}
	}
};