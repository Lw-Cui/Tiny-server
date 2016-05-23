_Pragma ("once");
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
	static int getConnfd(unsigned short);

private:
	static const int LISTENQ = 1024;
	static void Listening(unsigned short);
	static int listenfd;
};

class NetService {
public:
	ssize_t read_str(std::string &);
	ssize_t write_str(const std::string &str); 

protected:
	int connfd;

private:
	ssize_t rio_write(int fd, const char *usrbuf, size_t n);
	void rio_readline(int fd, std::string &usrbuf);
};

class Communicator:public NetService {
public:
	Communicator(int cfd);
	Communicator(const std::string &);
	~Communicator();

	void closeConnfd();

private:
	void Connect(std::string);
	int connfd;
};