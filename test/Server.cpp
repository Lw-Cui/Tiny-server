#include <NetService.hpp>
#include <algorithm>
#include <iostream>
#include <thread>
using namespace std;

void startServe(short connfd) {
	NetService se{connfd}; std::string buf; 
	while (se.read_str(buf)) {
		cout << "Recv: " << buf << endl;
		transform(buf.begin(), buf.end(), buf.begin(), ::toupper);
		se.write_str(buf);
	}
}

int main(int argc, char *argv[]) {
	unsigned short port = 2000;
	if (argc == 2) sscanf(argv[1], "%hu", &port);
	try {
		while(true) 
			std::thread(startServe, Server::getConnfd(port)).detach();
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}