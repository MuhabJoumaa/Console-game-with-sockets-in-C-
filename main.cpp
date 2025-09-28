#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize WinSock" << std::endl;
        return 1;
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(8888);
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to server" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected to the server. Please enter a number (or '#' to finish):" << std::endl;
    std::string input;
    while (true)
    {
        std::cin >> input;
        ssize_t bytesSent = send(clientSocket, input.c_str(), input.size(), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Failed to send number to server" << std::endl;
            closesocket(clientSocket);
            return 1;
        }
        if (input == "#")
        {
            break;
        }
    }
    char serverMessage[1024];
    memset(serverMessage, 0, sizeof(serverMessage));
    ssize_t bytesRead = recv(clientSocket, serverMessage, sizeof(serverMessage) - 1, 0);
    if (bytesRead == -1)
    {
        std::cerr << "Failed to receive response from server" << std::endl;
        closesocket(clientSocket);
        return 1;
    }
    std::cout << "Server response: " << serverMessage << std::endl;
    WSACleanup();
    return 0;
}
