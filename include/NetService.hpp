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
	NetService(int cfd = -1):connfd{cfd} {}
	NetService(const NetService&) = delete;
	NetService &operator=(const NetService&) = delete;
	NetService(const std::string &hostname, int port):connfd{-1} {
		open_connfd(hostname, port);}

	~NetService() {close(connfd);}

	ssize_t read_str(std::string &str) {
		str.clear(); rio_read(connfd, str); return str.length();}
	NetService& write_str(const std::string &str) {rio_write(connfd, str); return *this;}

private:
	int connfd;

	void open_connfd(const std::string &, int);
	void rio_write(int fd, const std::string& usrbuf);
	void rio_read(int fd, std::string &usrbuf);
};