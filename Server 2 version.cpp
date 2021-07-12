#include <iostream>
#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <map>

#pragma warning(disable: 4996)

std::map<uint64_t,SOCKET> Connections;

int ClientHandler(int index) {
	int32_t msg_size;
	int32_t recv_t = 0;
	while (true) {
		recv_t = recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		if (recv_t > 0)
		{
			char* msg = new char[msg_size + 1];
			msg[msg_size] = '\0';
			recv(Connections[index], msg, msg_size, NULL);
			for (int i = 0; i < Connections.size(); i++) {
				if (i == index) {
					continue;
				}
				send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
				send(Connections[i], msg, msg_size, NULL);
			}
			delete[] msg;
		}
		else
		{
			std::cout<<std::endl<<Connections.size()<<" индекс"<< Connections[index] <<std::endl;
			Connections.erase(index);
			return 0;
		}
	}
	Connections.erase(index);
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
	while(true) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else
		{
			auto index = Connections.size();
			std::cout << "Client Connected!\n";
			Connections.emplace(index, newConnection);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(index), NULL, NULL);
		}
	}

	system("pause");
	return 0;
}