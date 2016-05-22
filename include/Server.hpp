_Pragma ("once");
#include <jeayeson/jeayeson.hpp>
#include <exception>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

class server_error: public std::exception {
public:
	server_error(const std::string &p):str{p} {}
	const char *what() const noexcept {return str.c_str();}
private:
	std::string str;
};

class Server {
public:
	Server(unsigned short port);
	std::string read_str();
	ssize_t write_str(const std::string &str);
	void execute_CGI(const std::string &filename, const std::string &str);

private:
	int listenfd, connfd;
	static const int LISTENQ = 1024;
	ssize_t rio_write(int fd, const char *usrbuf, size_t n);
	void rio_readline(int fd, std::string &usrbuf);
};
