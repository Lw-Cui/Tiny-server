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

void err_sys(const char *fmt, ...);

void initLog(int argc, char **argv);

enum SocketType {
    TCP = SOCK_STREAM,
    UDP = SOCK_DGRAM,
};

class NetReadWrite {
public:
    static void rioWrite(int fd, const std::string &usrbuf);

    static size_t rioRead(int fd, std::string &usrbuf);
};

class Server {
public:
    Server(int p = 2000) : port{p} {}

    int waitConnection();

    int Listening(SocketType type = TCP);

    ~Server() {}

private:
    const int LISTENQ = 1024;
    int port;

protected:
    int listenfd = -1;
};

class Client {
public:
    ~Client() { close(connfd); }

    int getConnfd() const { return connfd; }

    int connectServer(const std::string &hostname, int port, SocketType type = TCP);

private:
    // Connection file descriptor
    int connfd = -1;
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

