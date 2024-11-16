#include <iostream>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

// Platform-specific setup and cleanup functions
void InitializeSockets() {
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed to initialize WinSock." << std::endl;
        exit(EXIT_FAILURE);
    }
#endif
}

void CleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

// Function to scan a single port
bool isPortOpen(const std::string &ip, int port) {
    struct sockaddr_in server;
    int sock;

#ifdef _WIN32
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return false;
    }
#else
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return false;
    }
#endif

    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Attempt to connect
    bool isOpen = (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0);

#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    return isOpen;
}

// Function to scan a range of ports
void scanPorts(const std::string &ip, int startPort, int endPort) {
    for (int port = startPort; port <= endPort; ++port) {
        if (isPortOpen(ip, port)) {
            std::cout << "Port " << port << " is open on " << ip << std::endl;
        } else {
            std::cout << "Port " << port << " is closed on " << ip << std::endl;
        }
    }
}

int main() {
    std::string targetIp;
    int startPort, endPort;

    std::cout << "Enter target IP address: ";
    std::cin >> targetIp;
    std::cout << "Enter start port: ";
    std::cin >> startPort;
    std::cout << "Enter end port: ";
    std::cin >> endPort;

    InitializeSockets();

    std::cout << "Scanning ports from " << startPort << " to " << endPort << " on " << targetIp << "...\n";
    scanPorts(targetIp, startPort, endPort);

    CleanupSockets();
    return 0;
}