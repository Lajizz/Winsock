// getappinfo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <cstring>
#pragma comment(lib,"Ws2_32.lib")

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

int main(int argc, char** argv)
{
    WSADATA wsaData;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    int iResult;
    if (argc != 3)
    {
        printf("usage: gethostaddress <hostname> <servicename>\n");
        printf("       prvides protocol-indepent translation\n");
        printf("       from an ANSI host name to an IP address");
        printf("gethostaddress example usage:\n");
        printf("gethostaddress www.contoso.com 0\n");
        return 1;
    }
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }



    int j = 1;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    printf("getaddinfo returned success.\n");
    struct sockaddr_in* sockaddr_ipv4;
    struct sockaddr_in6* sockaddr_ipv6;
    DWORD ipbufferlength = 46;
    char ipstringbuffer[46];

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        printf("getaddrinfo response %d\n", j++);

        wprintf(L"Flags: 0x%x\n", ptr->ai_flags);
        wprintf(L"Family: ");
        switch (ptr->ai_family) {
        case AF_UNSPEC: printf("AF_UNSPEC\n");break;
        case AF_INET:
            printf("AF_INET(IPv4)\n");
            sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            inet_ntop(AF_INET, &sockaddr_ipv4->sin_addr, ipstringbuffer, ipbufferlength);
            printf("IPv4: address: %s\n", ipstringbuffer);
            printf("Port: %d\n", ntohs(sockaddr_ipv4->sin_port));
            break;
        case AF_INET6:
            printf("AF_INET6(IPv6)\n");
            sockaddr_ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            inet_ntop(AF_INET, &sockaddr_ipv6->sin6_addr, ipstringbuffer, ipbufferlength);
            printf("IPv4: address: %s\n", ipstringbuffer);
            printf("Port: %d\n", ntohs(sockaddr_ipv6->sin6_port));
            break;
        case AF_NETBIOS: printf("AF_NETBIOS\n");break;
        default:
            printf("Other: %d\n", ptr->ai_family);
        }
        printf("Socket type: ");
        switch (ptr->ai_socktype) {
        case 0:
            printf("Unspecified\n"); break;
        case SOCK_STREAM:
            printf("SOCK_STREAM(stream)\n"); break;
        case SOCK_DGRAM:
            printf("SOCK_DGRAM(datagram)\n"); break;
        case SOCK_RAW:
            printf("SOCK_RAW(raw)\n"); break;
        case SOCK_RDM:
            printf("SOCK_RDM(reliable message datagram)\n"); break;
        case SOCK_SEQPACKET:
            printf("SOCK_SEQPACKET(pseudo-stream packet)\n"); break;
        default:
            printf("Other %d\n", ptr->ai_socktype); break;
        }
        printf("Protocol type: ");
        switch (ptr->ai_protocol) {
            case 0: printf("Unspecified\n");break;
            case IPPROTO_TCP: printf("TCP\n");break;
            case IPPROTO_UDP: printf("UDP\n");break;
            default: printf("Other %d\n", ptr->ai_protocol);break;

        }
        printf("Length of this sockaddr: %d\n", ptr->ai_addrlen);
        printf("Canonical name: %s\n", ptr->ai_canonname);
        printf("\n");
    }

    WSACleanup();
    return 0;
}
