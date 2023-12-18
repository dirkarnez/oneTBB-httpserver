#include <iostream>
#include <string>
#include <tbb/tbb.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const int DEFAULT_PORT = 8080;
const std::string RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, World!";

void ProcessRequest(SOCKET clientSocket) {
    send(clientSocket, RESPONSE.c_str(), RESPONSE.length(), 0);
    closesocket(clientSocket);
}

void ServerLoop() {
    struct addrinfo hints = {}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(nullptr, std::to_string(DEFAULT_PORT).c_str(), &hints, &result);

    SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(listenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    freeaddrinfo(result);

    listen(listenSocket, SOMAXCONN);

    while (true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        tbb::task::spawn([clientSocket]() {
            ProcessRequest(clientSocket);
        });
    }

    closesocket(listenSocket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ServerLoop();

    WSACleanup();
    return 0;
}
