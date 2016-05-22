#include <Server.hpp>
int main(int argc, char *argv[]) {
	unsigned short port = 2000;
	if (argc == 2) sscanf(argv[1], "%hu", &port);
	try {
		Server bop{port};
		while (true) {
			//json_map buf{json_data{bop.read_str()}};
			//bop.write_str(buf.to_string());
			bop.execute_CGI("CGI", bop.read_str());
		}
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}