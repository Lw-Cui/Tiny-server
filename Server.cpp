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
#define LISTENQ 1024

class server_error: public std::exception {
public:
	server_error(const char *p):str{p} {}
	const char* what() const noexcept {return str;}
private:
	const char *str;
};

class Server {
public:
	Server(unsigned short port) {
		int optval = 1;
		struct sockaddr_in serveraddr;

		if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw server_error("Make Socket failed.");

		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&optval, sizeof(int)) < 0)
			throw server_error("Setsockopt failed.");

		memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
		serveraddr.sin_port = htons(port);
		if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
			throw server_error("Bind socket to IP:port failed.");

		if (listen(listenfd, LISTENQ) < 0)
			throw server_error("Listen port failed.");
	}
	
	std::string read_str() {
	    struct sockaddr_in clientaddr;
		unsigned int clientlen = sizeof(clientaddr);
		if ((connfd = accept(listenfd, (sockaddr *)&clientaddr, &clientlen)) < 0)
			throw server_error("Accept failed.");

		/* Determine the domain name and IP address of the client */
	    struct hostent *hp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
				   sizeof(clientaddr.sin_addr.s_addr), AF_INET);
	    if (hp) throw server_error("Gethostbyaddr failed.");
		char *haddrp = inet_ntoa(clientaddr.sin_addr);
		printf("server connected to %s (%s)\n", hp->h_name, haddrp);

		std::string buf; rio_readline(connfd, buf);
		return std::move(buf);
	}

	ssize_t write_str(const std::string &str) {
		ssize_t size = rio_write(connfd, str.c_str(), str.length());
		if (close(connfd) < 0) throw server_error("Close failed."); return size;
	}

private:
	int listenfd, connfd;

	ssize_t rio_write(int fd, const char *usrbuf, size_t n) {
		size_t nleft = n; ssize_t nwritten; const char *bufp = usrbuf;
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

	ssize_t rio_readline(int fd, std::string &usrbuf) {
		unsigned num = 0, rc; char c; 
		while (true) {
			if ((rc = read(fd, &c, 1)) == 1) {
				usrbuf.push_back(c); if (c == '\r') break;
			} else if (rc == 0) {
				if (num == 1) return 0; else break;
			} else {
				throw server_error("Read failed.");
			}
			num++;
		}
		usrbuf.clear(); return num;
	}
};

int main(int argc, char *argv[]) {
	try {
		Server bop(10000);
		while (true) {
			std::string buf{bop.read_str()};
			bop.write_str(buf);
		}
	} catch(std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}