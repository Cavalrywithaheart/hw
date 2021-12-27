#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include "WinSock2.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
#define err(errMsg) printf("[line:%d]%s failed code %d\n",__LINE__,errMsg,WSAGetLastError())
#define ok(Msg) printf("%s Ok!\n\n",Msg)
using namespace std;

void init(string fileName);
void solve(SOCKET sessionSocket);
void sendFile(string fileName, string fileType, SOCKET sessionSocket);
string dataHead(string type, int size);

string filePos = "";
string _ip = "";
string _port = "";

int main() {
	init("C:/Users/25987/source/repos/computer_net/Socket/init.txt");
	WSADATA wsaData;
	//1.open lib
	if (WSAStartup(0x0202, &wsaData)) {
		err("Winsock  startup");
		return 0;
	}
	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
		WSACleanup();
		return 0;
	}
	ok("Winsock startup");

	//2.create socket
	SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKET sessionSocket;
	sockaddr_in addr, clientAddr;
	int addrLen;
	if (srvSocket == INVALID_SOCKET) {
		err("Create socket");
		return 0;
	}
	ok("Socket create");

	//3.set port and ip
	addr.sin_family = AF_INET;
	addr.sin_port = htons(stoi(_port));
	addr.sin_addr.S_un.S_addr = inet_addr(_ip.c_str());

	//4.binding
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));
	if (rtn == SOCKET_ERROR) {
		err("Socket bind");
		return 0;
	}
	ok("Socket bind");

	//5.listen
	rtn = listen(srvSocket, 50);
	if (rtn == SOCKET_ERROR) {
		err("Socket listen");
		return 0;
	}
	ok("Socket listen");

	clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);

	//6.accept
	while (true) {
		sessionSocket = accept(srvSocket, (sockaddr*)&clientAddr, &addrLen);
		if (sessionSocket == INVALID_SOCKET) {
			err("Accept error");
			return 0;
		}
		else {
			ok("Accept");
			thread sessionThread(solve, sessionSocket);//new thread
			printf("IP:%d.%d.%d.%d\n", (int)clientAddr.sin_addr.S_un.S_un_b.s_b1, (int)clientAddr.sin_addr.S_un.S_un_b.s_b2, (int)clientAddr.sin_addr.S_un.S_un_b.s_b3, (int)clientAddr.sin_addr.S_un.S_un_b.s_b4);
			printf("PORT:%d\n", clientAddr.sin_port);
			sessionThread.detach();
		}
	}
	return 0;
}

//read config file
void init(string fileName) {
	ifstream ifs;
	ifs.open(fileName, ios::in);
	if (!ifs.is_open())
	{
		printf("Configuration file not found\n");
		return;
	}
	char buf[1000] = { 0 };
	int line = 0;
	while (ifs.getline(buf, sizeof(buf)))
	{
		++line;
		if (line == 1) filePos = buf;
		else if (line == 2) _port = buf;
		else if (line == 3) _ip = buf;
	}
}

//send datagram
string dataHead(string contentType, int size) {
	stringstream str;
	str << "HTTP/1.1 200 OK\r\n"
		<< "Connection:keep-alive\r\n"
		<< "Server:Cavalry\r\n"
		<< "Content Length:" + to_string(size) + "\r\n"
		<< "Content Type:" + contentType + "\r\n\r\n";
	return str.str();
}

//send files
void sendFile(string fileName, string fileType, SOCKET sessionSocket) {
	ifstream file(filePos + fileName, ios::binary);
	if (!file) file.open(filePos + "404.html", ios::binary);
	filebuf* fileContent = file.rdbuf();
	int fileSize = fileContent->pubseekoff(0, file.end, file.in);
	string head = dataHead(fileType, fileSize);
	cout << head << endl;
	send(sessionSocket, head.c_str(), head.size(), 0);
	fileContent->pubseekpos(0, file.in);
	char* buf = new char[fileSize];
	char* tail = buf + fileSize;
	fileContent->sgetn(buf, fileSize);
	file.close();
	while (buf < tail) {
		int len = send(sessionSocket, buf, fileSize, 0);
		buf += len;
		fileSize -= len;
	}
	return;
}

void solve(SOCKET sessionSocket) {
	char recvBuf[4096];
	int retCode = recv(sessionSocket, recvBuf, 4096, 0);
	if (retCode == SOCKET_ERROR) {
		err("Recieve");
		return;
	}
	else {//parse
		stringstream ss;
		string fileName, type;
		ss.str(recvBuf);
		ss >> type >> fileName;
		cout << type << " " << fileName;
		ss >> type;
		cout << " " << type << endl;
		if (fileName.length() >= 1)
			cout << "Filename:" << fileName.substr(1) << endl;
		for (int i = 1; i <= 2; i++) {
			string a, b;
			ss >> a >> b;
			cout << a << " " << b << endl;
		}
		cout << endl;
		string suffixStr = fileName.substr(fileName.find_last_of('.') + 1);
		if (fileName.size() <= 1) {
			printf("Bad request!\n");
			closesocket(sessionSocket);
			return;
		}
		else if (suffixStr == "jpg" || suffixStr == "png")
			sendFile(fileName, "image/jpeg", sessionSocket);
		else if (suffixStr == "js")
			sendFile(fileName, "application/javascript", sessionSocket);
		else if (suffixStr == "css")
			sendFile(fileName, "text/css", sessionSocket);
		else if (suffixStr == "html" || suffixStr == "txt") {
			sendFile(fileName, "text/html", sessionSocket);
		}
		//		else if (suffixStr == "ico") {
		//			sendFile(fileName, "image/x-icon", sessionSocket);
		//		}
		else {
			cout << "Can not resolve " << fileName << endl;
			closesocket(sessionSocket);
			return;
		}
		closesocket(sessionSocket);
		cout << "Send file " << fileName << " OK!" << endl;
	}
}