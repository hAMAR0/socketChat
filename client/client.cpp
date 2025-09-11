#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <thread>


#pragma comment(lib, "Ws2_32.lib")

#define CON_PORT 8888
#define BUFFER_LEN 512


void uInput(char* buffer, int size) {
	std::cout << "Enter message: " << std::endl;
	fgets(buffer, size, stdin);
}


void socketShutdown(SOCKET& socket) {
	if (shutdown(socket, 1) == SOCKET_ERROR) {
		std::cout << WSAGetLastError() << std::endl;
		closesocket(socket);
		WSACleanup();
	}
}

void socketReceiving(SOCKET& socket) {
	char rBuffer[BUFFER_LEN];
	int result;
	do {
		result = recv(socket, rBuffer, BUFFER_LEN, 0);
		if (result > 0) {
			for (int i = 0; i < result; i++) {
				std::cout << rBuffer[i];
			}
		}
		else if (result == 0) std::cout << "Connection closed by server" << std::endl;
		else std::cout << "Receiving failed" << WSAGetLastError() << std::endl;
	} while (result > 0);


}

void sendData(SOCKET connectSocket, char* iBuffer) {
	// Send data
	const char* buffer = iBuffer;	//const because "testline" stored in read-only memory section

	if (send(connectSocket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
		closesocket(connectSocket);
		WSACleanup();
	}

	// Close connection FOR SENDING after all data was sent
	// 0 to stop receiving, 1 to stop sending, 2 for both
	socketShutdown(connectSocket);

	// Receiving data til server closes the connection
	socketReceiving(connectSocket);
}

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


	result = connect(connectSocket, reinterpret_cast<sockaddr*>(&saClient), sizeof(saClient));
	if (result == SOCKET_ERROR) {
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}
	else {
		std::cout << "Connection to server successful" << std::endl;
	}

	char buffer[BUFFER_LEN];
	uInput(buffer, BUFFER_LEN);
	sendData(connectSocket, buffer);

	closesocket(connectSocket);
	WSACleanup();
	return 0;
}