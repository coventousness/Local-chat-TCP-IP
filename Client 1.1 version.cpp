#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma warning(disable: 4996)

SOCKET Connection;

int ClientHandler() {

	return 0;
}
int main(int argc, char* argv[]) {
	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: failed connect to server.\n";
		return 1;
	}
	std::cout << "Connected!\n";

	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	std::string msg1;
	bool registration;
	uint32_t id{ 0 };
	char name[64];
	char password[64];
	std::cout << "You registration? yes->1 / not->0: ";
	std::cin >> registration;
	std::cout << registration;
	send(Connection, (char*)&registration, sizeof(bool), NULL);
	if (registration == 0)
	{
		std::cout << "Cin name: ";
		std::cin >> name;
		send(Connection, (char*)&name, sizeof(name), NULL);
		std::cout << "Cin password: ";
		std::cin >> password;
		send(Connection, (char*)&password, sizeof(password), NULL);
		recv(Connection, (char*)&id, sizeof(id), NULL);
		std::cout << "You id: " << id;
		std::cout << "\nCin id frendly: ";
		std::cin >> id;
		send(Connection, (char*)&id, sizeof(id), NULL);
	}
	else
	{
		std::cout << "Cin id: ";
		std::cin >> id;
		send(Connection, (char*)&id, sizeof(id), NULL);
		std::cout << "Cin password: ";
		std::cin >> password;
		send(Connection, (char*)&password, sizeof(password), NULL);
		std::cout << "\nCin id frendly: ";
		std::cin >> id;
		send(Connection, (char*)&id, sizeof(id), NULL);
	}
	while (true) {
		/*std::getline(std::cin, msg1);
		int32_t msg_size = msg1.size();
		if (msg_size > 0 && msg_size < 2147483645)
		{
			send(Connection, (char*)&msg_size, sizeof(int), NULL);
			send(Connection, msg1.c_str(), msg_size, NULL);
		}*/
		Sleep(10);
	}

	system("pause");
	return 0;
}