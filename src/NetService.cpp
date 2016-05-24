#include <NetService.hpp>
using namespace std;

void NetService::open_connfd(const std::string &hostname, int port) {
	struct hostent *hp;
	struct sockaddr_in serveraddr;
	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw server_error("Open socket error.");

	if ((hp = gethostbyname(hostname.c_str())) == NULL)
		throw server_error("Get hostname error.");
	memset(reinterpret_cast<char *>(&serveraddr), 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy(reinterpret_cast<char *>(&serveraddr.sin_addr.s_addr),
		reinterpret_cast<char *>(hp->h_addr_list[0]), hp->h_length);
	serveraddr.sin_port = htons(port);

	if (connect(connfd, reinterpret_cast<struct sockaddr *>(&serveraddr),
		 sizeof(serveraddr)) < 0)
		throw server_error("connect error.");
}

void NetService::rio_read(int fd, std::string &usrbuf) {
	while (true) {
		char c; long int rc{read(fd, &c, 1)};
		if (rc == 1) {usrbuf.push_back(c);} 
		if (rc == 0 || c == '\0') return; // EOF
		if (rc < 0) throw server_error("Read failed.");
	}
}

void NetService::rio_write(int fd, const string& usrbuf) {
	size_t nleft{usrbuf.length()}; ssize_t nwritten;
	const char *bufp{usrbuf.c_str()};
	while (nleft > 0) {
		if ((nwritten = write(fd, bufp, nleft)) <= 0) {
			if (errno == EINTR) nwritten = 0;
			else throw server_error("Write failed.");
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	write(fd, "\0", 1); // write EOF
}

int Server::listenfd = -1;
void Server::Listening(unsigned short port) {
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

int Server::getConnfd(unsigned short port) {
	if (listenfd == -1) Listening(port);
    struct sockaddr_in clientaddr;
	unsigned int clientlen = sizeof(clientaddr);
	int connfd = -1;
	if ((connfd = accept(listenfd, 
		reinterpret_cast<sockaddr *>(&clientaddr), &clientlen)) < 0)
		throw server_error("Accept failed.");
	return connfd;
}
