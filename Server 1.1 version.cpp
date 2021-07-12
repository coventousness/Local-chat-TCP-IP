#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma warning(disable: 4996)

SOCKET Connections[10000];
int Counter = 0;
struct user
{
	char name[64]{ 0 };
	uint32_t id{ 0 };
	char password[64]{ 0 };
}; // 132байт
user* users = new user[10000]{ 0 };

int ClientHandler(int socket_user1, int socket_user2)
{
	int32_t msg_size;
	int32_t recv_t1 = 0;
	int32_t recv_t2 = 0;
	while (true) {
		recv_t1 = recv(Connections[socket_user1], (char*)&msg_size, sizeof(int), NULL);
		if (recv_t1 > 0)
		{
			char* msg = new char[msg_size + 1];
			msg[msg_size] = '\0';
			recv(Connections[socket_user1], msg, msg_size, NULL);
			send(Connections[socket_user2], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[socket_user2], msg, msg_size, NULL);
			delete[] msg;
		}
		else
		{
			return 0;
		}
		recv_t2 = recv(Connections[socket_user2], (char*)&msg_size, sizeof(int), NULL);
		if (recv_t2 > 0)
		{
			char* msg = new char[msg_size + 1];
			msg[msg_size] = '\0';
			recv(Connections[socket_user2], msg, msg_size, NULL);
			send(Connections[socket_user1], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[socket_user1], msg, msg_size, NULL);
			delete[] msg;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
int User(int socket_user1)
{
	bool registration;
	uint32_t id{ 0 };
	char name[64];
	char password[64];
	bool id_true = false;
	bool password_true = false;
	recv(Connections[socket_user1], (char*)&registration, sizeof(bool), NULL);
	std::cout << registration<<std::endl;
	if (registration == 1)
	{
		recv(Connections[socket_user1], (char*)&id, sizeof(id), NULL);
		std::cout << id << std::endl;
		recv(Connections[socket_user1], (char*)&password, sizeof(password), NULL);
		std::cout << password << std::endl;
		for (int i = 1; i < 10001; i++)
		{
			if (!strcmp(password, users[i].password)&& id == users[i].id)
			{
				std::cout << "id_true" << std::endl;
				std::cout << "password_true" << std::endl;
				continue;
			}
		}
	}
	else
	{
		for (int z = 1; z < 10001; z++)
		{
			if (users[z].id == 0)
			{
				recv(Connections[socket_user1], (char*)&users[z].name, sizeof(users[z].name), NULL);
				std::cout << users[z].name;
				recv(Connections[socket_user1], (char*)&users[z].password, sizeof(users[z].password), NULL);
				std::cout << users[z].password;
				users[z].id = z;
				send(Connections[socket_user1], (char*)&z, sizeof(z), NULL);
				continue;
			}
		}
	}
	uint32_t socket_user2{0};
	while(recv(Connections[socket_user1], (char*)&socket_user2, sizeof(socket_user2), NULL)<0)
	ClientHandler(socket_user1, socket_user2);
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

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 1; i < 10001; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)User, (LPVOID)(i), NULL, NULL);
		}
	}
	system("pause");
	return 0;
}