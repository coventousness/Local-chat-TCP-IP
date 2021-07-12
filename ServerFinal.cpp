#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable: 4996)
#include "sqlite3.h"
#include <map>
#include <vector>
#include <string>
#include <thread>

sqlite3* db;

std::map<uint64_t, SOCKET> Connections;
std::vector<SOCKET> Temporary_Socket;

class Registration
{
private:
	bool registration;
	uint32_t id=0;
	char name[32]{0};
	char login[64]{ 0 };
	char password[64]{0};
	
	std::string OutMessage;
	std::string CheckUserData;
	std::string IntoRegisterData;
	std::string SlectRegisterData;

	bool user;
	char* err = 0;
	size_t size;
public:
	static int BackOutMessage(void* context, int columnCount, char** columnValues, char** columnNames)
	{
		int RECIPIENT = atoi(columnValues[0]);
		int SENDER = atoi(columnValues[1]);
		char NAME[32];  
		char MESSAGE[216]; 
		std::strcpy(NAME,columnValues[2]);
		std::strcpy(MESSAGE,columnValues[3]);
		auto conect=Connections.find(RECIPIENT);
		while (send(conect->second, (char*)&SENDER, sizeof(SENDER), 0) <= 0) { auto eorrr = WSAGetLastError(); std::cout << eorrr; if (eorrr != 10035) { break; } }
		while (send(conect->second, NAME, 32, 0) <= 0) { auto eorrr = WSAGetLastError(); std::cout << eorrr; if (eorrr != 10035) { break; } }
		while (send(conect->second, MESSAGE, 216, 0) <= 0) { auto eorrr = WSAGetLastError(); std::cout << eorrr; if (eorrr != 10035) { break; } }
		char* err = 0;
		std::string deleted ="DELETE FROM MESSAGE WHERE(RECIPIENT="+std::to_string(RECIPIENT)+" AND SENDER="+ std::to_string(SENDER) +" AND MESSAGE=\""+ MESSAGE +"\");";
		if (sqlite3_exec(db, deleted.c_str(), 0, 0, &err))
		{
			std::cout << "Помилка SQL deleted: " << err;
			sqlite3_free(err);
		}
		return 0;
	}
	int RegisterData(int columnCount, char** columnValues, char** columnNames)
	{
		registr.id = atoi(columnValues[0]) + 1;
		return 0;
	}
	static int CallbackRegister(void* context, int columnCount, char** columnValues, char** columnNames)
	{
		Registration* reg = reinterpret_cast<Registration*>(context);
		return reg->RegisterData(columnCount, columnValues, columnNames);
	}
	int CheckData(int columnCount, char** columnValues, char** columnNames)
	{
		registr.id = atoi(columnValues[0]);
		registr.user = true;
		return 0;
	}
	static int CallbackCheckData(void* context, int columnCount, char** columnValues, char** columnNames)
	{
		Registration* check = reinterpret_cast<Registration*>(context);
		return check->CheckData(columnCount, columnValues, columnNames);
	}
	void reg()
	{
		setlocale(LC_ALL, "ukr");
		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
		uint32_t size = 0;
		uint32_t i = 0;
		while (true)
		{
			size = Temporary_Socket.size();
			if (i < size)
			{
				int error = recv(Temporary_Socket[i], (char*)&registr.registration, sizeof(bool), 0);
				int getError = WSAGetLastError();
				if (error > 0)
				{
					if (registr.registration)
					{
						registr.user = false;
						while (recv(Temporary_Socket[i], registr.login, 64, 0) <= 0) {};
						while (recv(Temporary_Socket[i], registr.password, 64, 0) <= 0) {};

						CheckUserData = "SELECT* FROM PERSON WHERE(LOGIN=\"" + (std::string)registr.login + "\" AND PASSWORD=\"" + registr.password + "\");";
						if (sqlite3_exec(db, CheckUserData.c_str(), CallbackCheckData, 0, &err))
						{
							std::cout << "Помилка SQL CheckUserData: " << err;
							sqlite3_free(err);
						}
						if (registr.user == true)
						{
							while (send(Temporary_Socket[i], (char*)&registr.id, 4, 0) <= 0) {};
							Connections.emplace(registr.id, Temporary_Socket[i]);
							OutMessage = "SELECT* FROM MESSAGE WHERE(RECIPIENT=" + std::to_string(registr.id) + ");";
							if (sqlite3_exec(db, OutMessage.c_str(), BackOutMessage, 0, &err))
							{
								std::cout << "Помилка SQL OutMessage: " << err;
								sqlite3_free(err);
							}
							auto df = Temporary_Socket.begin();
							Temporary_Socket.erase(df + i);
						}
						else { while (send(Temporary_Socket[i], "", 4, 0) <= 0) {}; continue; }
					}
					else
					{
						registr.user = false;
						while (recv(Temporary_Socket[i], registr.login, 64, 0) <= 0) {};
						while (recv(Temporary_Socket[i], registr.name, 32, 0) <= 0) {};
						while (recv(Temporary_Socket[i], registr.password, 64, 0) <= 0) {};

						CheckUserData = "SELECT* FROM PERSON WHERE(LOGIN=\"" + (std::string)registr.login + "\");";
						if (sqlite3_exec(db, CheckUserData.c_str(), CallbackCheckData, 0, &err))
						{
							std::cout << "Помилка SQL CheckUserData: " << err;
							sqlite3_free(err);
						}
						if (registr.user == false)
						{
							SlectRegisterData = "SELECT ID FROM PERSON ORDER BY id DESC LIMIT 1;";
							if (sqlite3_exec(db, SlectRegisterData.c_str(), CallbackRegister, 0, &err))
							{
								std::cout << "Помилка SQL SlectRegisterData: " << err;
								sqlite3_free(err);
							}
							if (registr.id == 0) { ++registr.id; };

							IntoRegisterData = "INSERT INTO PERSON VALUES(" + std::to_string(registr.id) + ",\"" + registr.login + "\",\"" + registr.password + "\",\"" + registr.name + "\");";
							if (sqlite3_exec(db, IntoRegisterData.c_str(), 0, 0, &err))
							{
								std::cout << "Помилка SQL IntoRegisterData: " << err;
								sqlite3_free(err);
							}

							std::cout << registr.id << std::endl;
							while (send(Temporary_Socket[i], (char*)&registr.id, 4, 0) <= 0) {};
							Connections.emplace(id, Temporary_Socket[i]);
						}
						else { while (send(Temporary_Socket[i], "", 4, 0) <= 0) {}; continue; }
						auto df = Temporary_Socket.begin();
						Temporary_Socket.erase(df + i);
					}
				}
				else if (getError == WSAEWOULDBLOCK) {}
				else
				{
					auto df = Temporary_Socket.begin();
					Temporary_Socket.erase(df + i);
				}
				++i;
			}
			else { i = 0; }
		}
	}
}registr;
void messaging()
{
	setlocale(LC_ALL, "ukr");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	uint32_t id = 0;
	char name[32]{ 0 };
	char message[216]{ 0 };

	char* err = 0;
	std::string IntoOutMessage;
	while (true) 
	{
		for (auto& i : Connections)
		{

			if (recv(i.second, (char*)&id, sizeof(id), 0) > 0)
			{
				while (recv(i.second, name, sizeof(name), 0) <= 0) {}
				while (recv(i.second, message, sizeof(message), 0) <= 0) {}
				IntoOutMessage = "INSERT INTO MESSAGE VALUES("+ std::to_string(id) +","+ std::to_string(i.first) + ",\"" + name +"\",\""+message+"\");";
				uint32_t id_send = i.first;
				IDfrendly:
				auto IDfrendly = Connections.find(id);
				if (IDfrendly != Connections.end())
				{
					while (send(IDfrendly->second, (char*)&id_send, sizeof(id_send), 0) <= 0) { auto eorrr = WSAGetLastError(); std::cout << eorrr; if (eorrr != 10035) { Connections.erase(IDfrendly); goto IDfrendly; } }
					while (send(IDfrendly->second, name, sizeof(name), 0) <= 0) { auto eorrr = WSAGetLastError(); std::cout << eorrr; if (eorrr != 10035) { Connections.erase(IDfrendly); goto IDfrendly; } }
					while (send(IDfrendly->second, message, sizeof(message), 0) <= 0) { auto eorrr = WSAGetLastError(); std::cout << eorrr; if (eorrr != 10035) { Connections.erase(IDfrendly); goto IDfrendly; } }
				}
				else
				{
					if (sqlite3_exec(db, IntoOutMessage.c_str(), 0, 0, &err))
					{
						std::cout << "Помилка SQL IntoOutMessage: " << err;
						sqlite3_free(err);
					}
				}
			}
			else if (WSAGetLastError()==10035)
			{
				continue;
			}
			else
			{
				Connections.erase(i.first);
			}
		}
	}
}
int main(int argc, char* argv[])
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
	int size_addr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	SOCKET sending;

	char* err = 0;
	if (sqlite3_open("database_server", &db))
	{
		std::cout << "Помилка вiдкриття/створення БД: " << sqlite3_errmsg(db);
	}
	std::string Table_Person = "CREATE TABLE IF NOT EXISTS PERSON(ID INT PRIMARY KEY NOT NULL, LOGIN CHAR[64] NOT NULL, PASSWORD CHAR[64] NOT NULL, NAME CHAR[32] NOT NULL);";
	if (sqlite3_exec(db, Table_Person.c_str(), 0, 0, &err))
	{
		std::cout<<"Помилка SQL Table_Person: "<< err;
		sqlite3_free(err);
	}
	std::string Table_Message = "CREATE TABLE IF NOT EXISTS MESSAGE(RECIPIENT INT NOT NULL, SENDER INT NOT NULL, NAME CHAR[32] NOT NULL, MESSAGE CHAR[216] NOT NULL);";
	if (sqlite3_exec(db, Table_Message.c_str(), 0, 0, &err))
	{
		std::cout << "Помилка SQL Table_Message: " << err;
		sqlite3_free(err);
	}
	
	u_long ioctl = 1;
	
	std::thread reg([]() {registr.reg(); });
	std::thread messagin([]() {messaging(); });

	while (true)
	{
		newConnection = accept(sListen, (SOCKADDR*)&addr, &size_addr);
		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else 
		{
			sending = newConnection;
			ioctlsocket(sending, FIONBIO, &ioctl);
			Temporary_Socket.emplace_back(sending);
		}
	}
	reg.join();
	messagin.join();
	sqlite3_close(db);
	atexit([]() { });
}
