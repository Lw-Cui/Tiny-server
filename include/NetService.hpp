_Pragma ("once");

#include <exception>
#include <iostream>
#include <algorithm>
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

void initLog(int argc, char **argv);

enum SocketType {
    TCP = SOCK_STREAM,
    UDP = SOCK_DGRAM,
};

class Server {
public:
    Server(int p = 2000, SocketType t = TCP) : port{p}, type{t} {}

    int waitConnection();

    int Listening();


    ~Server() {}

private:
    const int LISTENQ = 1024;
    int port;
    SocketType type;

protected:
    int listenfd = -1;
};

class IOMultiplexingServer : public Server {
public:
    IOMultiplexingServer(int port) : Server{port} {
        maxfd = listenfd = Listening();
        maxfd++;
        FD_ZERO(&socketSet);
        FD_SET(listenfd, &socketSet);
        LOG(DEBUG) << "Start IO multiplexing";
    }

    std::vector<int> getReadyClient();

    std::vector<int> getClient() const {
        return clientVec;
    }

private:
    void addClient(int fd) {
        FD_SET(fd, &socketSet);
        clientVec.push_back(fd);
        maxfd = std::max(maxfd, fd);
    }

    fd_set socketSet;
    int maxfd;
    // clientVec should not contain listenfd
    std::vector<int> clientVec;
};

class Client {
public:
    Client(const std::string &hostname, int port, SocketType t = TCP) : type{t} {
        connectServer(hostname, port);
    }

    Client(const Client &) = delete;

    Client &operator=(const Client &) = delete;

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

    int getConnfd() const { return connfd; }

protected:
    // Connection file descriptor
    int connfd = -1;
    SocketType type;
private:
    void connectServer(const std::string &, int);

};

class IOMultiplexingClient : public Client {
public:
    IOMultiplexingClient(const std::string &hostname, int port) : Client{hostname, port} {
        FD_ZERO(&socketSet);
        FD_SET(connfd, &socketSet);
        FD_SET(0, &socketSet);
        LOG(DEBUG) << "Start client IO multiplexing";
    }


    void loop(std::function<void(const std::string &)> f);

private:
    fd_set socketSet;
};

class server_error : public std::exception {
public:
    server_error(const std::string &p) : str{p} {}

    const char *what() const noexcept { return str.c_str(); }

private:
    std::string str;
};
