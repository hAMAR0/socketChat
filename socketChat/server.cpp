#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")

#define LISTEN_PORT 8888
#define MAX_CON 1
#define BUFFER_LEN 512



int main() {
	int result;
	// WS2_32.dll initialization 
	WSADATA wsaData;
	int wsaResult;

	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (wsaResult != 0) {
		std::cout << "WS2_32.dll is missing" << std::endl;
		return 0;
	}
	else {
		std::cout << "DLL Status: " << wsaData.szSystemStatus << std::endl;
	}


	// Open socket
	SOCKET serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPv4, TCP
	if (serverSocket == INVALID_SOCKET) {
		std::cout << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}


	// Bind socket
	sockaddr_in saServer;
	saServer.sin_family = AF_INET;		// Use IPv4
	//saServer.sin_addr.s_addr = INADDR_ANY;		// Allow connections from any IP
	InetPton(AF_INET, L"127.0.0.1", &saServer.sin_addr.s_addr); // use InetPton or inet_pton instead of deprecated inet_addr


	// sin_addr is a union so there are different ways to acces it:
	// s_un_b (four 1-byte integers), s_un_w (two 2-bytes integers) or as s_addr (one 4-bytes integer)


	saServer.sin_port = htons(LISTEN_PORT);	// from big-endian to little-endian (network byte order)

	//int bindResult = bind(serverSocket, (sockaddr*)&saServer, sizeof(saServer));	// (sockaddr*)&saServer - casts sockaddr_in sa Server to generic sockaddr C style
	result = bind(serverSocket, reinterpret_cast<sockaddr*>(&saServer), sizeof(saServer));	// using c++ style cast instead
	if (result == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Socket bound on port " << LISTEN_PORT << std::endl;
	}


	// Listen on socket
	result = listen(serverSocket, MAX_CON);
	if (result == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Listening on socket with maximum amount of simultaneous connections:  " << MAX_CON << std::endl;
	}


	// Accept the connection
	SOCKET acceptSocket;
	acceptSocket = accept(serverSocket, NULL, NULL);
	if (acceptSocket == INVALID_SOCKET) {
		std::cout << WSAGetLastError() << std::endl;
		closesocket(serverSocket);;
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Accepted the connection request" << std::endl;
	}


	// Receive data til the peer shuts down

	char receiveBuffer[BUFFER_LEN];
	do {
		result = recv(acceptSocket, receiveBuffer, BUFFER_LEN, 0);

		if (result > 0) {
			std::cout << "Bytes received - " << result << std::endl;

			if (send(acceptSocket, receiveBuffer, result, 0) == SOCKET_ERROR) {		// echo back
				std::cout << "Echo failed" << std::endl;
				closesocket(acceptSocket);
				WSACleanup();
				return 0;
			}
		}
		else if (result == 0) {
			std::cout << "Closing connection" << std::endl;
		}
		else {
			std::cout << "Receive failed";
			closesocket(acceptSocket);;
			WSACleanup();
			return 0;
		}
	} while (result > 0);


	closesocket(acceptSocket);
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}