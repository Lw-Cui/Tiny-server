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
#include <easylogging++.h>

void rioWrite(int fd, const std::string &usrbuf);

void rioRead(int fd, std::string &usrbuf);

class server_error : public std::exception {
public:
    server_error(const std::string &p) : str{p} {}

    const char *what() const noexcept { return str.c_str(); }

private:
    std::string str;
};


class Server {
public:
    int waitConnection(unsigned short);

private:
    const int LISTENQ = 1024;

    void Listening(unsigned short);

    int listenfd = -1;
};

class Client {
public:
    Client(int cfd) : connfd{cfd} {}

    Client(const Client &) = delete;

    Client &operator=(const Client &) = delete;

    Client(const std::string &hostname, int port) {
        connectServer(hostname, port);
    }

    ~Client() { close(connfd); }

    ssize_t readStr(std::string &str) {
        str.clear();
        rioRead(connfd, str);
        return str.length();
    }

    Client &writeStr(const std::string &str) {
        rioWrite(connfd, str);
        return *this;
    }

private:
    // Connection file descriptor
    int connfd = -1;

    void connectServer(const std::string &, int);

};