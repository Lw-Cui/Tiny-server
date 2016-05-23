#include <NetService.hpp>

void NetService::rio_readline(int fd, std::string &usrbuf) {
	while (true) {
		char c; long int rc{read(fd, &c, 1)};
		if (rc == 1) {usrbuf.push_back(c);} 
		if (rc == 0 || c == '\n') return; // EOF
		if (rc < 0) throw server_error("Read failed.");
	}
}

ssize_t NetService::rio_write(int fd, const char *usrbuf, size_t n) {
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

ssize_t NetService::read_str(std::string &buf) {
	rio_readline(connfd, buf); return buf.length();
}

ssize_t NetService::write_str(const std::string &str) {
	ssize_t size{rio_write(connfd, str.c_str(), str.length())};
	return size;
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
