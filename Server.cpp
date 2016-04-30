#include <jeayeson/jeayeson.hpp>
#include "restclient-cpp/restclient.h"
#include <exception>
#include <iostream>
#include <string>
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
	Server(unsigned short port) {
		int optval = 1;
		struct sockaddr_in serveraddr;

		if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw server_error("Socket open failed.");

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&optval, sizeof(int)) < 0)
			throw server_error("Setsockopt failed.");

		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
		serveraddr.sin_port = htons(port);
		if (bind(listenfd, reinterpret_cast<sockaddr *>(&serveraddr),
		 sizeof(serveraddr)) < 0)
			throw server_error("Bind socket failed.");

		if (listen(listenfd, LISTENQ) < 0)
			throw server_error("Listen port failed.");
	}
	
	std::string read_str() {
	    struct sockaddr_in clientaddr;
		unsigned int clientlen = sizeof(clientaddr);
		if ((connfd = accept(listenfd, 
			reinterpret_cast<sockaddr *>(&clientaddr), &clientlen)) < 0)
			throw server_error("Accept failed.");

		/* Determine the domain name and IP address of the client */
	    struct hostent *hp = gethostbyaddr(
	    	reinterpret_cast<const char *>(&clientaddr.sin_addr.s_addr), 
		    sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	    if (!hp) throw server_error("Gethostbyaddr failed.");
#ifdef DEBUG
		char *haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);
#endif //DEBUG

		std::string buf; rio_readline(connfd, buf);
		return std::move(buf);
	}

	ssize_t write_str(const std::string &str) {
		ssize_t size{rio_write(connfd, str.c_str(), str.length())};
		if (close(connfd) < 0) throw server_error("Close failed."); 
		return size;
	}

private:
	int listenfd, connfd;
	static const int LISTENQ = 1024;

	ssize_t rio_write(int fd, const char *usrbuf, size_t n) {
		size_t nleft{n}; ssize_t nwritten; const char *bufp{usrbuf};
		while (nleft > 0) {
			if ((nwritten = write(fd, bufp, nleft)) <= 0) {
				if (errno == EINTR) nwritten = 0;
				else throw server_error("Write failed.");
			}
			nleft -= nwritten;
			bufp += nwritten;
		}
		return n;
	}

	void rio_readline(int fd, std::string &usrbuf) {
		while (true) {
			char c; int rc{read(fd, &c, 1)};
			if (rc == 1) {usrbuf.push_back(c);} 
			if (rc == 0 || c == '\n') return; // EOF
			if (rc < 0) throw server_error("Read failed.");
		}
	}
};

	
int main(int argc, char *argv[]) {

	const std::string u{"http://oxfordhk.azure-api.net/academic/v1.0/evaluate?expr=Id=2140251882&count=10000&attributes=Id,AA.AuId,AA.AfId&subscription-key=f7cc29509a8443c5b3a5e56b0e38b5a6"};
	RestClient::Response r = RestClient::get(u.c_str());
	std::cout << r.body << std::endl;

	unsigned short port = 2000;
	if (argc == 2) sscanf(argv[1], "%hu", &port);
	try {
		Server bop{port};
		while (true) {
			json_map buf{json_data{bop.read_str()}};
			bop.write_str(buf.to_string());
		}
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
