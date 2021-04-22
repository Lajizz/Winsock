// gethostname.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"Ws2_32.lib")

int main(int argc, char** argv)
{
    WSADATA wsaData;
    struct sockaddr_in saGHN;
    char* hostname = new char[NI_MAXHOST];
    char* servInfo = new char[NI_MAXSERV];
    saGHN.sin_family = AF_INET;
    if (argc == 1) {
        saGHN.sin_port = htons(0);
        inet_pton(AF_INET, "127.0.0.1", &saGHN.sin_addr.s_addr);

    }
    else if(argc == 2) {
        saGHN.sin_port =htons(80);
        inet_pton(AF_INET, argv[1], &saGHN.sin_addr.s_addr);
    }
    else if(argc == 3) {
        u_short tmp;
        sscanf_s(argv[2], "%hu", &tmp);
        saGHN.sin_port = htons(tmp);
        inet_pton(AF_INET, argv[1], &saGHN.sin_addr.s_addr);
    }
    else {

    }
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    int dwRetval;
    
    dwRetval = getnameinfo((struct sockaddr*)&saGHN, sizeof(struct sockaddr), hostname, NI_MAXHOST, servInfo, NI_MAXSERV, NI_NAMEREQD);
    if (dwRetval != 0) {
        printf("getnameinfo failed with error # %ld\n", WSAGetLastError());
        return 1;
    }
    else {
        printf("getnameinfo returned hostname = %s\n", hostname);
        if(argc>=2){
            printf("getnameinfo returned servInfo = %s\n", servInfo);
        }



    }
    delete []hostname;
    delete[]servInfo;
    WSACleanup();




    return 0;
}

