#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define CON_PORT 8888

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


	// Creating a socket for connection to the server
	SOCKET connectSocket = INVALID_SOCKET;
	connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connectSocket == INVALID_SOCKET) {
		std::cout << WSAGetLastError() << std::endl;
		WSACleanup();
		return 0;
	}

	
	// Connecting to the server

	sockaddr_in saClient;
	saClient.sin_family = AF_INET;		// Use IPv4
	InetPton(AF_INET, L"127.0.0.1", &saClient.sin_addr.s_addr);
	saClient.sin_port = htons(CON_PORT);


	int conResult = connect(connectSocket, reinterpret_cast<sockaddr*>(&saClient), sizeof(saClient));
	if (conResult == SOCKET_ERROR) {
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}
	else {
		std::cout << "Connection to server successful" << std::endl;
	}


	closesocket(connectSocket);
	WSACleanup();
	return 0;
}