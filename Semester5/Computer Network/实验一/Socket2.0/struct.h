#pragma once
#include <string>
#include <map>

using namespace std;

struct Config {
	// default config
	string url;
	int port;
	string dir;
	Config() {
		url = "127.0.0.1";
		port = 5050;
		dir = "C:/Users/25987/source/repos/computer_net/Socket2.0/www/";
	}
};