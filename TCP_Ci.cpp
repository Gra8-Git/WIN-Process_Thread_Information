// TCPCli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// TCPClinet.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <string>
#include <vector>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <thread>
#include <regex>
#pragma comment(lib, "Ws2_32.lib")
struct Object {
	int m_nId;
	wchar_t m_szName[32];
	int m_nType;
	std::string server_ip;
	int server_port;
};

class Client {
private:
	std::string server_ip;
	int server_port;
	std::vector<Object> objects;
	int next_id = 0;
	SOCKET ConnectSocket;
	SOCKET ListenSocket;
	SOCKET connectedclientSocket;
	int bufferSize_priv;
public:
	void connect_s(std::string ip, int port) {
		server_ip = ip;
		server_port = port;
		WSTA();
		sock_cli();
		if (connect_too(ConnectSocket, ip, port) == 0) {
			std::cout << "[+]Connected to " << server_ip << ":" << server_port << std::endl;
		}
		else { std::cout << "[-]NOT Connected to " << server_ip << ":" << server_port << std::endl; }
	}

	unsigned char* serialize(Object obj)
	{
		bufferSize_priv = sizeof Object;
		unsigned char buffer[sizeof Object];
		memcpy(buffer, &obj, bufferSize_priv);
		return buffer;
	}

	Object deserialize(unsigned char* buffer)
	{
		Object obj;
		memcpy(&obj, buffer, bufferSize_priv);
		return obj;
	}

	void create(int m_nType, wchar_t m_szName[32]) {
		Object obj;
		obj.m_nId = next_id++;
		obj.m_nType = m_nType;

		wcscpy_s(obj.m_szName, 32, m_szName);
		objects.push_back(obj);
		std::wcout << "Created object with id " << obj.m_nId << " and name " << obj.m_szName << std::endl;
		auto buffer = serialize(obj);

		if (check_connection_state(ConnectSocket) == 0) {
			obj.server_ip = server_ip;
			obj.server_port = server_port;
			send_sock(ConnectSocket, buffer, bufferSize_priv);
			std::cout << "object send to :" << server_ip << ":" << server_port << std::endl;
		}
	}

	void destroy(int id) {
		Object obj;

		if (objects.size() == 0) { std::cout << "Number of objects: 0" << std::endl; }
		else {
			for (int i = 0; i < objects.size(); i++) {
				if (objects[i].m_nId == id) {
					objects.erase(objects.begin() + i);
					std::cout << "Destroyed object with id " << id << std::endl;
					return;
				}
			}
		}
		std::cout << "Object with id " << id << " not found" << std::endl;
	}

	void status() {
		Object obj;
		if (check_connection_state(ConnectSocket) != 1|| check_connection_state(ConnectSocket) != 1) {
			std::cout << "Connected to " << server_ip << ":" << server_port << std::endl;
		}
		
		
		if (objects.size() == 0) { std::cout << "Number of objects: 0" << std::endl; }
		else {
			for (int i = 0; i < objects.size(); i++) {
				std::cout << "Object with id " << i << " , ";
			}
				std::cout << "Number of objects: " << objects.size() << std::endl;
		}
		
		std::cout << std::endl;
		
	}





	void disconnect_s() {

		//destroy objects 
		Object obj;

		if (objects.size() == 0) { std::cout << "Number of objects: 0" << std::endl; }
		else {
			for (int i = 0; i < objects.size(); i++) {

				objects.erase(objects.begin() + i);
				std::cout << "Destroyed object with id " << i << std::endl;
				return;

				//std::cout << "Object with id " << i << " not found" << std::endl;
			}
		}

		//discon ect
		if (check_connection_state(ConnectSocket) != 1 || check_connection_state(ConnectSocket) != 1) {
			disconnect(ConnectSocket);
			std::cout << "Disconnected from " << server_ip << ":" << server_port << std::endl;
		}
		else {
			std::cout << "No connectinons " << std::endl;
		}
	}
	
	

	int WSTA()
	{
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			std::cerr << "WSAStartup failed: " << iResult << std::endl;
			return 1;
		}
	}

	int sock_cli()
	{
		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET)
		{
			std::cerr << "socket() error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}
	}

	int sock_server()
	{
		ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET)
		{
			std::cerr << "socket() error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}
	}

	int socket_bind(int port)
	{
		sockaddr_in service;
		service.sin_family = AF_INET;
		service.sin_addr.s_addr = INADDR_ANY;
		service.sin_port = htons(port);

		int bindResult = bind(ListenSocket, (SOCKADDR*)&service, sizeof(service));
		if (bindResult == SOCKET_ERROR) {
			std::cout << "Bind failed:" << WSAGetLastError();
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		return 0;
	}

	int listen_serv()
	{
		int listenResult = listen(ListenSocket, SOMAXCONN);
		if (listenResult == SOCKET_ERROR) {
			std::cout << "Listen failed: " << WSAGetLastError();
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	int accept_serv()
	{
		connectedclientSocket = accept(ListenSocket, NULL, NULL);
		if (connectedclientSocket == INVALID_SOCKET) {
			std::cout << "Accept failed: " << WSAGetLastError();
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	int connect_too(SOCKET ConnectSocket, std::string ip, int port)
	{
		sockaddr_in serverAddress;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		//unsigned long ip_address = 
		//serverAddress.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr);
		int iResult = connect(ConnectSocket, (sockaddr*)&serverAddress, sizeof serverAddress);
		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "Error at connect(): " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	int send_sock(SOCKET ConnectSocket, unsigned char buffer[], int bufferSize)
	{
		int sent_bytes = 0;
		int total_sent = 0;
		int iResult = 0;


		// iResult =send(ConnectSocket, (char*)bufferSize, 1024, 0);
		 if (iResult == SOCKET_ERROR)
		 {
			 std::cerr << "send failed: " << WSAGetLastError() << std::endl;
			 closesocket(ConnectSocket);
			 WSACleanup();
			 return 1;
		 }
		 else {
		
				 std::cout << "Send failed" << std::endl;
				 return 1;
	
		 }

		while (total_sent < bufferSize)
		{
		iResult = send(ConnectSocket, (const char*)buffer + total_sent, bufferSize - total_sent, 0);
		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "send failed: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		else {
			if (sent_bytes == -1)
				{
				std::cout << "Send failed" << std::endl;
				return 1;
				}
			total_sent += sent_bytes;

			}
		}

		std::cout << "Sent: " << buffer << std::endl;
		return 0;
	}

	int disconnect(SOCKET ConnectSocket) {
		int iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			std::cerr << "Error at shutdown(): " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	int check_connection_state(SOCKET ConnectSocket)
	{
		int iResult;
		int val = 0;
		int valSize = sizeof(val);

		iResult = getsockopt(ConnectSocket, SOL_SOCKET, SO_ERROR, (char*)&val, &valSize);
		if (iResult == SOCKET_ERROR)
		{
		//	std::cerr << "getsockopt failed: " << WSAGetLastError() << std::endl;
			return 1;
		}

		if (val == 0)
		{
		//	std::cout << "The socket is connected." << std::endl;
			return 0;
		}
		else
		{
			//std::cerr << "The socket is not connected. Error code: " << val << std::endl;
			return 1;
		}
	}


};



//DWORD WINAPI ThreadProc(LPVOID lpParameter) {
	
void ThreadProc() {
std::cout << "Thread running in background" << std::endl;

	Client client;
	std::string command;
	wchar_t m_szName[32];
	int m_nType;
	int m_nId;

	while (command != "exit") {
		std::cout << "> ";
		std::cin >> command;


		if (command == "connect") {
			std::string ip;
			int port = 0;
			std::string input;
			std::cout << "<IPv4:Port> --connect to a server" << std::endl;
			std::cin >> input;

			std::size_t pos = input.find(":");
			if (pos != std::string::npos)
			{
				ip = input.substr(0, pos);
				port = std::stoi(input.substr(pos + 1));
			}
			else
			{
				std::cerr << "Invalid input" << std::endl;
				break;
			}

			//check is it ip or not 
			std::regex pattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
			std::smatch match;

			if (std::regex_search(ip, match, pattern) && match.size() > 1) {
				std::cout << "Connection  IP: " << ip << " Port: " << port << std::endl;
				client.connect_s(ip, port);
			}else{
				std::cout << "Not IPv4 address" << std::endl;
			}
		
		}
		else if (command == "create") {
			std::cout << "<type> <name> --create an object with the specified data, and transmit it to the server : " << std::endl;
			std::wcin >> m_nType >> m_szName;

			std::wcout << "ID: " << m_nType << std::endl;
			std::wcout << "Name: " << m_szName << std::endl;
			client.create(m_nType, m_szName);   
		}
		else if (command == "destroy") {
			std::cout << "[+]  <object id> -- destroy the object assigned with the specified identifier : " << std::endl;
			std::cin >> m_nId;
			client.destroy(m_nId);
		}
		else if (command == "status") {
			std::cout << "[+] status -- print information ip and port if connected and all objects" << std::endl;

			client.status();
		}
		else if (command == "disconnect") {
			std::cout << "disconnect -- should request / notify the server about termination, destroy all objects : " << std::endl;
			client.disconnect_s();
		}
		else if (command == "exit") {
			std::cout << "exit -- request / notify the server about termination, destroy all objects: " << std::endl;
			client.disconnect_s();
			std::cout << "application terminated" << std::endl;
			exit(0);
		}
		else {
			std::cout << "command unknown" << std::endl;
		}
	}
	
	//return 0;
}

int main(int argc, char* argv[]) {


	std::thread t(ThreadProc);

	std::cout << "Main thread continues to run" << std::endl;
	t.join();


	/*
	HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	if (hThread == NULL) {
		std::cerr << "Failed to create thread" << std::endl;
		return 1;
	}

	std::cout << "Main thread continues to run" << std::endl;
	WaitForSingleObject(hThread, INFINITE);
	*/

	//end


	
	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

