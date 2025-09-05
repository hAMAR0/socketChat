#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>


#pragma comment(lib, "Ws2_32.lib")

#define LISTEN_PORT 8888
#define MAX_CON 1


int main() {

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
	int bindResult = bind(serverSocket, reinterpret_cast<sockaddr*>(&saServer), sizeof(saServer));	// using c++ style cast instead
	if (bindResult == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Socket bound on port " << LISTEN_PORT << std::endl;
	}


	// Listen on socket
	int listenResult = listen(serverSocket, MAX_CON);
	if (listenResult == SOCKET_ERROR) {
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
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		std::cout << "Accepted the connection request" << std::endl;
	}


	closesocket(serverSocket);
	WSACleanup();
	return 0;
}