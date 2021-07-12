#include <stdio.h>
#include <tchar.h>
#include <string>
#include <thread>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable: 4996)
#include "sqlite3.h"
sqlite3* db;
SOCKET Connection;
std::string name;

namespace Handler
{
	uint32_t id = 0;
}

int CallbackLastID(void* context, int columnCount, char** columnValues, char** columnNames)
{
	Handler::id = atoi(columnValues[0]) + 1;
	return 0;
}
int ClientHandler() 
{
	setlocale(LC_ALL, "ukr");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	uint32_t id = 0;
	char name[32]{ 0 };
	char message[216]{ 0 };

	char* err = 0;
	std::string Table_Message;

	while (true)
	{
		recv(Connection, (char*)&id, sizeof(id), 0);
		recv(Connection, name, sizeof(name), 0);
		recv(Connection, message, sizeof(message), 0);
		std::cout << name << ":" << message << std::endl;

		Table_Message = "CREATE TABLE IF NOT EXISTS ID" + std::to_string(id) + " (ID INT PRIMARY KEY NOT NULL, SENDER INT NOT NULL, MESSAGE CHAR[216] NOT NULL);";
		if (sqlite3_exec(db, Table_Message.c_str(), 0, 0, &err))
		{
			std::cout << "Помилка SQL Table_Message CREATE: " << err;
			sqlite3_free(err);
		}

		Table_Message = "SELECT ID FROM ID" + std::to_string(id) + " ORDER BY id DESC LIMIT 1;";
		if (sqlite3_exec(db, Table_Message.c_str(), CallbackLastID, 0, &err))
		{
			std::cout << "Помилка SQL Table_Message SELECT: " << err;
			sqlite3_free(err);
		}

		Table_Message = "INSERT INTO ID" + std::to_string(id) + " VALUES(" + std::to_string(Handler::id) + "," + std::to_string(id) + ",\"" + message + "\");";
		if (sqlite3_exec(db, Table_Message.c_str(), 0, 0, &err))
		{
			std::cout << "Помилка SQL Table_Message INSERT: " << err;
			sqlite3_free(err);
		}
	}
	return 0;
}
int registration()
{
	bool registration;
	uint32_t id{ 0 };
	char name[32];
	char login[64];
	char password[64];
	while (true)
	{
		int registration_time = 0;
		std::cout << "You registration? yes-> 1 / not-> 0: ";
		std::cin >> registration_time;
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
		}
		else if (registration_time == 1 || registration_time == 0)
		{
			registration = registration_time;
			break;
		}
		else
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
		}
	}
	if (!registration)
	{
		char* err = 0;
		std::cout << "Cin login: ";
		std::cin >> login;
		std::cout << "Cin name: ";
		std::cin >> name;
		std::cout << "Cin password: ";
		std::cin >> password;
		send(Connection, (char*)&registration, sizeof(bool), NULL);
		send(Connection, (char*)login, sizeof(login), NULL);
		send(Connection, (char*)name, sizeof(name), NULL);
		send(Connection, (char*)password, sizeof(password), NULL);
		recv(Connection, (char*)&id, sizeof(id), NULL);
		std::cout << "You id: " << id;
		std::string Table_MyName = "INSERT INTO NAMES VALUES("+std::to_string(id)+",\""+name+"\");";
		if (sqlite3_exec(db, Table_MyName.c_str(), 0, 0, &err))
		{
			std::cout << "Помилка SQL Table_MyName: " << err;
			sqlite3_free(err);
		}
	}
	else
	{
		std::cout << "Cin login: ";
		std::cin >> login;
		std::cout << "Cin password: ";
		std::cin >> password;
		send(Connection, (char*)&registration, sizeof(bool), NULL);
		send(Connection, (char*)&login, sizeof(login), NULL);
		send(Connection, (char*)&password, sizeof(password), NULL);
		recv(Connection, (char*)&id, sizeof(id), NULL);
		std::cout << "You id: " << id;
	}
	return id;
}
int Name(void* context, int columnCount, char** columnValues, char** columnNames)
{
	name = columnValues[0];
	std::cout << name <<std::endl;
	return 0;
}
int main()
{
	setlocale(LC_ALL, "ukr");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
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
	char* err = 0;
	if (sqlite3_open("database", &db))
	{
		std::cout << "Помилка вiдкриття/створення БД: " << sqlite3_errmsg(db);
	}
	std::string Table_Name = "CREATE TABLE IF NOT EXISTS NAMES(ID INT PRIMARY KEY NOT NULL, NAME CHAR[32] NOT NULL);";
	if (sqlite3_exec(db, Table_Name.c_str(), 0, 0, &err))
	{
		std::cout << "Помилка SQL Table_Person: " << err;
		sqlite3_free(err);
	}

	uint32_t MyID = registration();
	std::thread Handler([]() {ClientHandler(); });

	uint32_t IDfrendly = 0;
	char name_char[32]{ 0 };
	char message[216]{ 0 };

	std::string MyName = "SELECT NAME FROM NAMES WHERE(ID=" + std::to_string(MyID) + ");";
	if (sqlite3_exec(db, MyName.c_str(), Name, 0, &err))
	{
		std::cout << "Помилка SQL MyName: " << err;
		sqlite3_free(err);
	}
	std::strcpy(name_char, name.c_str());

	std::string Table_Message;
	std::string str_message;
	for (;;)
	{
		std::cout << name_char <<std::endl;
		while (true)
		{
			uint32_t IDfrendly_time = 0;
			std::cout << "\nCin id frendly: ";
			std::cin >> IDfrendly_time;
			if (std::cin.fail())
			{
				std::cin.clear();
				std::cin.ignore(32767, '\n');
			}
			else if (IDfrendly_time > 0 && IDfrendly_time < 4294967294)
			{
				IDfrendly = IDfrendly_time;
				break;
			}
			else
			{
				std::cin.clear();
				std::cin.ignore(32767, '\n');
			}
		}
		for (;;)
		{	
			while (true) { std::getline(std::cin, str_message); if (str_message.size() > 0) { if (str_message.size() < 216) { break; } else { std::cout << "Maximum characters 215\n"; } } }

			std::strcpy(message, str_message.c_str());
			send(Connection, (char*)&IDfrendly, sizeof(IDfrendly), 0);
			send(Connection, name_char, sizeof(name_char), 0);
			send(Connection, message, sizeof(message), 0);

			Table_Message = "CREATE TABLE IF NOT EXISTS ID" + std::to_string(IDfrendly) + " (ID INT PRIMARY KEY NOT NULL, SENDER INT NOT NULL, MESSAGE CHAR[216] NOT NULL);";
			if (sqlite3_exec(db, Table_Message.c_str(), 0, 0, &err))
			{
				std::cout << "Помилка SQL Table_Message: " << err;
				sqlite3_free(err);
			}

			Table_Message = "SELECT ID FROM ID" + std::to_string(IDfrendly) + " ORDER BY id DESC LIMIT 1;";
			if (sqlite3_exec(db, Table_Message.c_str(), CallbackLastID, 0, &err))
			{
				std::cout << "Помилка SQL Table_Message SELECT: " << err;
				sqlite3_free(err);
			}

			Table_Message = "INSERT INTO ID" + std::to_string(IDfrendly) + " VALUES(" + std::to_string(Handler::id) +","+ std::to_string(0) + ",\"" + message + "\");";
			if (sqlite3_exec(db, Table_Message.c_str(), 0, 0, &err))
			{
				std::cout << "Помилка SQL Table_Message: " << err;
				sqlite3_free(err);
			}
		}
	}
	Sleep(1000);
	Handler.join();
	atexit([](){});
}