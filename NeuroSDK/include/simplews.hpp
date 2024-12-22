#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <cstdio>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class WebSocket {
public:
    enum class Opcode : uint8_t {
        CONTINUATION = 0x0,
        TEXT = 0x1,
        BINARY = 0x2,
        CLOSE = 0x8,
        PING = 0x9,
        PONG = 0xA
    };

    using MessageCallback = std::function<void(const std::string&)>;

private:
    SOCKET socket_fd = INVALID_SOCKET;
    MessageCallback on_message;

    bool send_all(const uint8_t* buffer, size_t length) {
        size_t total = 0;
        size_t bytes_left = length;
        int sent;

        while (total < length) {
            sent = ::send(socket_fd, (char*)buffer + total, (int)bytes_left, 0);
            if (sent == SOCKET_ERROR) return false;
            total += sent;
            bytes_left -= sent;
        }
        return true;
    }

public:
    WebSocket() {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~WebSocket() {
        if (socket_fd != INVALID_SOCKET) {
            closesocket(socket_fd);
            WSACleanup();
        }
    }

    bool connect(const std::string& url) {
        struct addrinfo* result = NULL, hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Assume URL is in format "host:port"
        size_t pos = url.find(":");
        std::string host = url.substr(0, pos);
        std::string port = url.substr(pos + 1);

        int iResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
        if (iResult != 0) {
            return false;
        }

        for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
            socket_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (socket_fd == INVALID_SOCKET) {
                continue;
            }

            iResult = ::connect(socket_fd, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(socket_fd);
                socket_fd = INVALID_SOCKET;
                continue;
            }
            break;
        }

        freeaddrinfo(result);

        if (socket_fd == INVALID_SOCKET) {
            return false;
        }

        // WebSocket handshake would go here
        // This is highly simplified; you would need to craft the actual HTTP Upgrade request here
        std::string handshake = "GET / HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
            "Sec-WebSocket-Version: 13\r\n\r\n";
        if (!send_all((uint8_t*)handshake.c_str(), handshake.length())) {
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
            return false;
        }

        // Check for successful upgrade response
        // This is pseudo-code; real implementation would involve more detailed parsing
        char buffer[1024];
        if (recv(socket_fd, buffer, sizeof(buffer) - 1, 0) == SOCKET_ERROR) {
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
            return false;
        }
        buffer[sizeof(buffer) - 1] = '\0';
        if (strstr(buffer, "101 Switching Protocols") == NULL) {
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
            return false;
        }

        return true;
    }

    void set_on_message(MessageCallback callback) {
        on_message = callback;
    }

    bool send(const std::string& message, Opcode opcode = Opcode::TEXT) {
        uint8_t* sendBuffer = new uint8_t[20 + message.length()];
        //set message to not fragmented, type as text, and no special flags
        sendBuffer[0] = 0b10000001;
        uint8_t offset = 0;
        size_t lengthOfMessage = message.length();
        //set the mask to true on each of these(first bit set to 1)
        if( lengthOfMessage <= 125)
        {
            sendBuffer[1] = 0b10000000 + (uint8_t)message.length();
        }
        else if(( lengthOfMessage >= 125) && (lengthOfMessage <= SHRT_MAX))
        {
            // Set mask to true and length to 126
            sendBuffer[1] = 0b11111110;
            sendBuffer[2] = (uint8_t)(lengthOfMessage >> 8);
            sendBuffer[3] = (uint8_t)(lengthOfMessage);
            offset = 2;
        }
        else {
            // Set the length to 127 and the mask to true
            sendBuffer[1] = 0xff;
            // Depending on the world size_t is either 32 or 64 bits
            // So at compile time determine the size of size_t
            // and then set the appropriate bytes
#if SIZE_MAX == UINT32_MAX
            sendBuffer[2] = 0;
            sendBuffer[3] = 0;
            sendBuffer[4] = 0;
            sendBuffer[5] = 0;
#else
            // Set the first byte to 0 since we are only using 32 bits
            sendBuffer[2] = (uint8_t)((lengthOfMessage >> 56) & 0xff);
            sendBuffer[3] = (uint8_t)((lengthOfMessage >> 48) & 0xff);
            sendBuffer[4] = (uint8_t)((lengthOfMessage >> 40) & 0xff);
            sendBuffer[5] = (uint8_t)((lengthOfMessage >> 32) & 0xff);
#endif
            sendBuffer[6] = (uint8_t)((lengthOfMessage >> 24) & 0xff);
            sendBuffer[7] = (uint8_t)((lengthOfMessage >> 16) & 0xff);
            sendBuffer[8] = (uint8_t)((lengthOfMessage >> 8));
            sendBuffer[9] = (uint8_t)(lengthOfMessage & 0xff);
            offset = 8;
        }
        //generate the masking key
        std::random_device randomDevice;
        std::mt19937 mersenneTwister;
        std::uniform_int_distribution<uint16_t> distribution(0, 255);
        sendBuffer[2 + offset] = (char)distribution(mersenneTwister);
        sendBuffer[3 + offset] = (char)distribution(mersenneTwister);
        sendBuffer[4 + offset] = (char)distribution(mersenneTwister);
        sendBuffer[5 + offset] = (char)distribution(mersenneTwister);

        //add the data to the toSend array
        for (int i = 0; i < message.length(); i++)
        {
            //xor the byte with the masking key to "mask" the message
            sendBuffer[6 + offset + i] = message[i] ^ sendBuffer[2 + offset + i % 4];
        }
        
        bool res = send_all(sendBuffer, (size_t)offset + 6 + message.length());
        delete[] sendBuffer;
        return res;
    }

    void receive(std::string *stringBuffer) {
        if (socket_fd == INVALID_SOCKET) return;

        char socketBuffer[2];
        int bytesRecieved1 = ::recv(socket_fd, socketBuffer, sizeof(socketBuffer),0);
        uint8_t payloadLengthSimple = socketBuffer[1] & 0b01111111; //get the seven least significant bits
        uint64_t payloadLength=0;
        if (payloadLengthSimple <= 125)
        {
            payloadLength = payloadLengthSimple;
        }
        else if (payloadLengthSimple == 126)
        {
            uint8_t payloadLengthBuffer[2];
            ::recv(socket_fd, (char*)payloadLengthBuffer, sizeof(payloadLengthBuffer), 0);
            payloadLength = (uint64_t)payloadLengthBuffer[0] << 8;
            payloadLength += (uint64_t)payloadLengthBuffer[1];
        }
        else if (payloadLengthSimple == 127)
        {
            uint8_t payloadLengthBuffer[8];
            ::recv(socket_fd, (char*)payloadLengthBuffer, sizeof(payloadLengthBuffer), 0);
            payloadLength = (uint64_t)payloadLengthBuffer[0] << 56;
            payloadLength += (uint64_t)payloadLengthBuffer[1] << 48;
            payloadLength += (uint64_t)payloadLengthBuffer[2] << 40;
            payloadLength += (uint64_t)payloadLengthBuffer[3] << 32;
            payloadLength += (uint64_t)payloadLengthBuffer[4] << 24;
            payloadLength += (uint64_t)payloadLengthBuffer[5] << 16;
            payloadLength += (uint64_t)payloadLengthBuffer[6] << 8;
            payloadLength += (uint64_t)payloadLengthBuffer[7];
        }
        char* textBuffer = new char[payloadLength + 1];
        uint32_t bytesRecieved = 0;
        while (bytesRecieved < payloadLength)
        {
            bytesRecieved += ::recv(socket_fd, textBuffer + bytesRecieved, payloadLength - bytesRecieved, 0);
        }
        textBuffer[payloadLength] = '\0';
        *stringBuffer = std::string(textBuffer, bytesRecieved);
        delete[] textBuffer;
    }

    void close() {
        if (socket_fd != INVALID_SOCKET) {
            shutdown(socket_fd, SD_SEND);
            closesocket(socket_fd);
            socket_fd = INVALID_SOCKET;
        }
    }
};

#endif // WEBSOCKET_HPP