#include <Server.hpp>

int Server::listenfd = -1;
void Server::StartListening(unsigned short port) {
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
	if (listenfd == -1) StartListening(port);
    struct sockaddr_in clientaddr;
	unsigned int clientlen = sizeof(clientaddr);
	int connfd = -1;
	if ((connfd = accept(listenfd, 
		reinterpret_cast<sockaddr *>(&clientaddr), &clientlen)) < 0)
		throw server_error("Accept failed.");
	return connfd;
}

ssize_t Server::read_str(std::string &buf) {
	buf = read_str();
	return buf.length();
}

std::string Server::read_str() {
	std::string buf; rio_readline(connfd, buf);
	return std::move(buf);
}

void Server::rio_readline(int fd, std::string &usrbuf) {
	while (true) {
		char c; long int rc{read(fd, &c, 1)};
		if (rc == 1) {usrbuf.push_back(c);} 
		if (rc == 0 || c == '\n') return; // EOF
		if (rc < 0) throw server_error("Read failed.");
	}
}

ssize_t Server::write_str(const std::string &str) {
	ssize_t size{rio_write(connfd, str.c_str(), str.length())};
	return size;
}

void Server::closeConnfd() {
	if (close(connfd) < 0) throw server_error("Close failed."); 
}

void Server::execute_CGI(const std::string &filename, const std::string &str) {
	pid_t pid; char *const argv[] = {NULL};
	if ((pid = fork()) == 0) {
		setenv("JSON_STRING", str.c_str(), 1);
		if (dup2(connfd, STDOUT_FILENO) < 0)
			throw server_error("Dup failed.");
		if (execve(filename.c_str(), argv, environ) < 0) 
			throw server_error("Execve failed.");
	} 
	if (pid < 0) throw server_error("Fork failed."); 
	if (wait(NULL) < 0) throw server_error("Wait failed."); 
	if (close(connfd) < 0) throw server_error("Close failed."); 
}


ssize_t Server::rio_write(int fd, const char *usrbuf, size_t n) {
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
