#include "Server.h"
#include "util.h"

void Server::start() {
	//1.open lib
	if (WSAStartup(0x0202, &wsaData)) {
		err("Winsock  startup");
		return;
	}
	if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
		WSACleanup();
		return;
	}
	ok("Winsock startup");

	//2.create socket
	srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srvSocket == INVALID_SOCKET) {
		err("Create socket");
		return;
	}
	ok("Socket create");

	//3.set port and ip
	addr.sin_family = AF_INET;
	addr.sin_port = htons(config.port);
	addr.sin_addr.S_un.S_addr = inet_addr(config.url.c_str());

	//4.binding
	int rtn = bind(srvSocket, (LPSOCKADDR)&addr, sizeof(addr));
	if (rtn == SOCKET_ERROR) {
		err("Socket bind");
		return;
	}
	ok("Socket bind");

	//5.listen
	rtn = listen(srvSocket, 50);
	if (rtn == SOCKET_ERROR) {
		err("Socket listen");
		return;
	}
	ok("Socket listen");

	clientAddr.sin_family = AF_INET;
	addrLen = sizeof(clientAddr);

	//6.accept
	while (true) {
		sessionSocket = accept(srvSocket, (sockaddr*)&clientAddr, &addrLen);
		if (sessionSocket == INVALID_SOCKET) {
			err("Accept error");
			return;
		}
		else {
			ok("Accept");
			printf("IP:%d.%d.%d.%d\n", (int)clientAddr.sin_addr.S_un.S_un_b.s_b1, (int)clientAddr.sin_addr.S_un.S_un_b.s_b2, (int)clientAddr.sin_addr.S_un.S_un_b.s_b3, (int)clientAddr.sin_addr.S_un.S_un_b.s_b4);
			printf("PORT:%d\n", clientAddr.sin_port);
			solve();
		}
	}
	return;
}

void Server::solve() {
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
			sendFile(fileName, "image/jpeg");
		else if (suffixStr == "js")
			sendFile(fileName, "application/javascript");
		else if (suffixStr == "css")
			sendFile(fileName, "text/css");
		else if (suffixStr == "html" || suffixStr == "txt") {
			sendFile(fileName, "text/html");
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

void Server::sendFile(string fileName, string fileType) {
	ifstream file(config.dir + fileName, ios::binary);
	if (!file) file.open(config.dir + "404.html", ios::binary);
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