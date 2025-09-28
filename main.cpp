#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 4

std::string convertToString(char* clientMessage)
{
    std::string str = clientMessage;
    return str;
}

void CalculateWinner(SOCKET clients[], int clientCount)
{
    double numbers[MAX_CLIENTS], means[MAX_CLIENTS];
    double sum = 0, mean = 0, average;
    char clientMessage[1024];
    memset(clientMessage, 0, sizeof(clientMessage));
    bool breakTheWhile = false;
    while (!breakTheWhile)
    {
        for (int i = 0; i < clientCount; i++)
        {
            recv(clients[i], clientMessage, sizeof(clientMessage) - 1, 0);
            std::string clientMessageStr = convertToString(clientMessage);
            try
            {
                numbers[i] = stod(clientMessageStr);
                std::cout << "From client" << i << ": " << numbers[i] << std::endl;
                sum += numbers[i];
            }
            catch (const std::exception&)
            {
                breakTheWhile = true;
                break;
            }
        }
    }
    average = static_cast<double>(sum) / clientCount;
    for (int i = 0; i < clientCount; i++)
    {
        mean = numbers[i];
        double sumOfOthers = 0;
        for (int j = 0; j < clientCount; j++)
        {
            if (i != j)
            {
                sumOfOthers += numbers[j];
            }
        }
        mean = abs(mean - sumOfOthers / (clientCount - 1));
        means[i] = mean;
    }
    int winnerIndex = 0;
    double minimumMean = means[0];
    for (int i = 0; i < clientCount; i++)
    {
        if (means[i] < minimumMean)
        {
            minimumMean = means[i];
            winnerIndex = i;
        }
    }
    //std::cout << winnerIndex << std::endl;
    for (int i = 0; i < clientCount; i++)
    {
        if (i == winnerIndex)
        {
            std::string message = "You won! The mean value is " + std::to_string(means[i]) + ".\nAverage = " + std::to_string(average) + ".";
            ssize_t bytesSent = send(clients[i], message.c_str(), message.size(), 0);
            if (bytesSent == -1)
            {
                std::cerr << "Failed to send number to server" << std::endl;
                closesocket(clients[i]);
            }
        }
        else
        {
            std::string message = "You lose! The mean value is " + std::to_string(means[i]) + ".\nAverage = " + std::to_string(average) + ".";
            ssize_t bytesSent = send(clients[i], message.c_str(), message.size(), 0);
            if (bytesSent == -1)
            {
                std::cerr << "Failed to send number to server" << std::endl;
                closesocket(clients[i]);
            }
        }
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize WinSock" << std::endl;
        return 1;
    }
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    std::cout << "Waiting for clients to connect..." << std::endl;
    SOCKET clientSockets[MAX_CLIENTS];
    int clientCount = 0;
    while (clientCount < MAX_CLIENTS)
    {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Failed to accept client connection" << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
        clientSockets[clientCount] = clientSocket;
        std::cout << "Client" << clientCount << " connected" << std::endl;
        clientCount++;
        if (clientCount == MAX_CLIENTS)
        {
            std::cout << "All clients connected. Waiting for numbers..." << std::endl;
            CalculateWinner(clientSockets, clientCount);
            break;
        }
    }
    for (int i = 0; i < clientCount; i++)
    {
        closesocket(clientSockets[i]);
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
