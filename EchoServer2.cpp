#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <stdio.h>

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
int Count;
void do_service(void* client_s);
int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;
    unsigned int         client_s;        // Client socket descriptor
    struct sockaddr_in   client_addr;     // Client Internet address
    struct in_addr       client_ip_addr;  // Client IP address
    int                  addr_len;        // Internet address length
    char 		ipstringbuffer[46];
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }


    Count = 0; //number of thread
    while (1) // Main loop (Loop forever)
    {
        printf("Count=%d \n", Count++);
        // Accept a connection.  The accept() will block and then return with client_addr filled-in.
        addr_len = sizeof(client_addr);
        client_s = accept(ListenSocket, (struct sockaddr*)&client_addr, &addr_len);

        // Copy the four-byte client IP address into an IP address structure
        //   - See winsock.h for a description of struct in_addr
        memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

        // Print an informational message that accept completed
        printf("Connection %d accepted!!! \n", Count);
        inet_ntop(AF_INET, &client_ip_addr, ipstringbuffer, sizeof(ipstringbuffer));
        printf("\tClient socket number: %d\n", client_s);
        printf("\tIPv4 address: %s\n", ipstringbuffer);
        printf("\tPort nuber: %d\n", ntohs(client_addr.sin_port));

        if (_beginthread(do_service, 4096, (void*)client_s) < 0)
        {
            printf("ERROR - Unable to create thread \n");
            exit(1);
        }
    }
    while (Count); // Never reached!!! // Wait for all threads to finish
    closesocket(ListenSocket); // Close open sockets
    WSACleanup(); // This stuff cleans-up winsock

    WSACleanup();

    return 0;
}


void do_service(void* client_s)
{
    char                 out_buf[1024];    // Output buffer for GET request
    char                 in_buf[1024];     // Input buffer for response
    unsigned int         retcode;              // Return code
    unsigned int         i;                    // Loop counter

    printf("thread beninning... \n");

    int n = 0;
    // Loop until client shut down
    while (1)
    {
        // Receive from the client
        n = recv((unsigned int)client_s, in_buf, sizeof(in_buf), 0);
        if ( n <=0)
            break; // when client shut down
        printf("Received from client %d data = '%s' byte %d\n", (unsigned int)client_s, in_buf,n);

        // Echo the received message to the client
        send((unsigned int)client_s, in_buf, (strlen(in_buf) + 1), 0);
    }

    printf("thread completed... \n");
    // Decrement for a completed thread
    Count--;

    // Close all open sockets and end the thread
    closesocket((unsigned int)client_s);

    _endthread();
}
