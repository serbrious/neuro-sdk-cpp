#pragma once
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif


class NetworkHelper {
    public:
        NetworkHelper() {Initialize();};
        ~NetworkHelper() {Cleanup();};
        
    private:
        static void Initialize() {
#ifdef _WIN32
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                throw std::runtime_error("WSAStartup failed");
            }
#endif
        }
        static void Cleanup() {
            #ifdef _WIN32
                WSACleanup();
            #endif
        }
};









