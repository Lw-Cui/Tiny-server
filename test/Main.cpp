#include <Server.hpp>
#include <thread>

void startServe(unsigned short connfd) {
	Server bop{connfd}; std::string buf;
	while (bop.read_str(buf)) bop.write_str(buf);
	bop.closeConnfd();
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