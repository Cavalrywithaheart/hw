#include "struct.h"
#include "util.h"
#include "server.h"

using namespace std;

void run_server() {
	Config config = read_conf("config.ini");

	Server server(config);
	server.start();
}

int main() {
	run_server();
	return 0;
}