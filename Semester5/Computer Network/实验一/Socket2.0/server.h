#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#define BUF_SIZE 4096
#define WINSOCK_VER 0x0202
#include "WinSock2.h"
#include "struct.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <fstream>

#pragma comment(lib,"ws2_32.lib")
using namespace std;

class Server
{
public:
	Config config;

	void start();
	void solve();
	Server(Config conf) {
		config = conf;
	}
protected:
	WSADATA wsaData;
	SOCKET srvSocket;
	SOCKET sessionSocket;
	sockaddr_in addr, clientAddr;
	int addrLen;

	void sendFile(string fileName, string fileType);
};