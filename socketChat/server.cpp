#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <fstream>
#include <vector>
#include <map>
#include <mutex>



#pragma comment(lib, "Ws2_32.lib")

#define LISTEN_PORT 8888
#define MAX_CON 50
#define BUFFER_LEN 512

struct Clients
{
	SOCKET socket = NULL;
	std::string name;
	std::string currentRoom;
};

class ChatRoom {
public:
	std::map<std::string, std::vector<Clients>> roomClients;
};

std::vector<ChatRoom> rooms;
ChatRoom defaultRoom;


// adding client's socket to a room
void addtoRoom(SOCKET& clientSocket, std::string roomName) {
	Clients client;
	client.currentRoom = roomName;
	client.socket = clientSocket;
	//defaultRoom.roomClients["default"].push_back(client);
}

// extracting message from package and writing it to file

std::mutex fileMutex;

void writeFile(char buffer[], int size) {
	std::lock_guard<std::mutex> lock(fileMutex);
	std::ofstream chat("../chat", std::ios_base::app);
	for (int i=0; i < size; i++) {
		chat << buffer[i];
	}
	chat << '\n';
	chat.close();
}

void handleClient(SOCKET clientSocket) {	// add chatrooms creation, add echoing within chatrooms only
	char receiveBuffer[BUFFER_LEN];
	int result;
	do {
		result = recv(clientSocket, receiveBuffer, BUFFER_LEN, 0);

		if (result > 0) {
			if (strncmp(receiveBuffer, "/room", 6)) {
				std::string rname = "";
				for (int i = 6; i < result; i++) {
					rname = rname + receiveBuffer[i];
				}



			}
			else {
				writeFile(receiveBuffer, result);
				for (auto& client : defaultRoom.roomClients["default"]) {
					if (client.socket != clientSocket) {
						if (send(client.socket, receiveBuffer, result, 0) == SOCKET_ERROR) {
							break;
						}
					}
				}
			}
		}
		else if (result == 0) {
			std::cout << "Closing connection" << std::endl;
		}
		else {
			break;
		}
	} while (result > 0);


	for (auto& client : defaultRoom.roomClients["default"]) {
		if (client.socket == clientSocket) {
			shutdown(clientSocket, SD_SEND);
			closesocket(clientSocket);
		}
	}
}

int main() {

	rooms.push_back(defaultRoom);

	int result;
	// WS2_32.dll initialization 
	WSADATA wsaData;
	int wsaResult;

	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (wsaResult != 0) {
		std::cout << "WS2_32.dll is missing" << std::endl;
		return 0;
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

	while (true) {
		SOCKET clientSocket = accept(serverSocket, NULL, NULL);
		if (clientSocket != INVALID_SOCKET) {
			addtoRoom(clientSocket, "default");

			std::thread	clientThread(handleClient, clientSocket);	 // Receive data on a thread til the peer shuts down
			clientThread.detach();
		}
	}

	closesocket(serverSocket);
	WSACleanup();
	return 0;
}