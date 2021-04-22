// UdpChatting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <process.h>
#pragma comment(lib,"Ws2_32.lib")
#define DEFAULT_BUFLEN 512

void do_recv(void*);
int  main(int argc,char** argv)
{
    WSADATA wsaData;
    int iResult;
    char hostname[NI_MAXHOST];
    SOCKET ListenSocket = INVALID_SOCKET;
    char sendf[DEFAULT_BUFLEN];
    char ipstringbuffer[46];
    int port;

    struct addrinfo* result = NULL,
        * result_remote = NULL,
        hints;
    struct sockaddr_in host;
    struct sockaddr_in server;
    struct sockaddr_in client;
    int addrlen = sizeof(server);

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        printf("WSAStartup falied: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("hostname: %s\n", hostname);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;


    if (argc != 4) {
        printf("usage: uppchatting localport remote_ip remote_port\n");
        WSACleanup();
        return 1;
    }
    

    iResult = getaddrinfo(NULL, argv[1], &hints, &result);
    if (iResult != 0) {
        printf("get local addrinfo failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    memcpy(&host, result->ai_addr, sizeof(host));
    inet_ntop(AF_INET, &host.sin_addr, ipstringbuffer, sizeof(ipstringbuffer));
    port = ntohs(host.sin_port);
    printf("\tServer Address: %s\n", ipstringbuffer);
    printf("\tServer Port Number: %d\n", port);

    //create socket()
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) {
        printf("socket create failed: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    //get server_addr, client addr;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = host.sin_addr.s_addr;
    server.sin_port = host.sin_port;

    //bind()
    iResult = bind(ListenSocket, (struct sockaddr*)&server, sizeof(server));
    if (iResult == SOCKET_ERROR) {
        printf("bind failed: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    if (_beginthread(do_recv, 4096, (void*)ListenSocket) < 0) {
        printf("create thread failed\n");
        exit(1);
    }
    printf("sent part\n");


    client.sin_family = AF_INET;
    inet_pton(AF_INET, argv[2], &client.sin_addr.s_addr);
    sscanf_s(argv[3], "%hu", &port);
    client.sin_port = htons(port);

    while (1) {
        printf("put a string:\n");
        gets_s(sendf);
        if (sendto(ListenSocket, sendf, sizeof(sendf), 0, (struct sockaddr*)&client, addrlen) == SOCKET_ERROR) {
            printf("send failed.\n");
        }
        else {
            
        }
    }

    freeaddrinfo(result_remote);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}


void do_recv(void* value) {
    char recvf[DEFAULT_BUFLEN];
    printf("recv part\n");
    struct sockaddr_in src;
    int addrlen = sizeof(src);
    while (1) {
        if (recvfrom((SOCKET)value, recvf, sizeof(recvf), 0, (struct sockaddr*)&src, (int*)&addrlen)==SOCKET_ERROR) {
           
        }
        else {
            printf("recv %s\n",recvf);
        }
    }
    
    _endthread();
}