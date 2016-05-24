#include <NetService.hpp>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	unsigned short port = 2000;
	if (argc == 2) sscanf(argv[1], "%hu", &port);
	try {
		NetService se("localhost", port);
		std::string buf;
		while (cin >> buf) {
			se.write_str(buf).read_str(buf); cout << buf << endl;
		}
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}