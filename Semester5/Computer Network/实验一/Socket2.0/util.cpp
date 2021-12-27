#include "util.h"
#include "INIReader.h"


Config read_conf(string config_path) {
	//parse configuration
	INIReader reader(config_path);
	Config conf;

	if (reader.ParseError() < 0) {
		cout << "Parse file " << config_path << " error!" << endl;
		exit(1);
	}
	conf.url = reader.GetString("socket", "url", "http://127.0.0.1");
	conf.port = reader.GetInteger("socket", "port", 80);
	conf.dir = reader.GetString("socket", "dir", "C:/Users/25987/source/repos/computer_net/Socket2.0/dir/");

	return conf;
}

string dataHead(string contentType, int size) {
	stringstream str;
	str << "HTTP/1.1 200 OK\r\n"
		<< "Connection:keep-alive\r\n"
		<< "Server:Cavalry\r\n"
		<< "Content Length:" + to_string(size) + "\r\n"
		<< "Content Type:" + contentType + "\r\n\r\n";
	return str.str();
}