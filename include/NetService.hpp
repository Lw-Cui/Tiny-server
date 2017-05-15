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
#include <unordered_map>
#include <memory>
#include <functional>

void rioWrite(int fd, const std::string &usrbuf);

void rioRead(int fd, std::string &usrbuf);

void err_sys(const char *fmt, ...);

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

class Client {
public:
    Client(SocketType t = TCP) : type{t} {
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

    int connectServer(const std::string & hostname, int port);

private:
    // Connection file descriptor
    int connfd = -1;
    SocketType type;
};


class IOMultiplexingUtility {
public:
    IOMultiplexingUtility() {
        FD_ZERO(&socketSet);
    }

    void addFd(int fd, std::function<void(int)> action) {
        maxfd = fd + 1;
        FD_SET(fd, &socketSet);
        fdVec[fd] = action;
    }

    void setDefaultAction(std::function<void(int)> action) {
        defaultAction = action;
    }

    void start();

    std::vector<int> getUnspecifedFd();

private:
    fd_set socketSet;
    int maxfd;
    std::unordered_map<int, std::function<void(int)>> fdVec;
    std::function<void(int)> defaultAction;
};

