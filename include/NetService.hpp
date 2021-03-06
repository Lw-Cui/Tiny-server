_Pragma ("once");

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
#include <unordered_map>
#include <memory>
#include <functional>
#include <exception>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

void err_sys(const char *fmt, ...);

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
    int waitConnection();

    int Listening(int port, SocketType type = TCP);

    ~Server() {}

private:
    const int LISTENQ = 1024;

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
    IOMultiplexingUtility() : maxfd(INT_MIN) {
        FD_ZERO(&socketSet);
    }

    void addFd(int fd, std::function<void(int)> action) {
        maxfd = std::max(maxfd, fd);
        FD_SET(fd, &socketSet);
        fdVec[fd] = action;
    }

    void removeFd(int fd) {
        FD_CLR(fd, &socketSet);
        fdVec.erase(fd);
    }

    void setDefaultAction(std::function<void(int)> action) {
        defaultAction = action;
    }

    void processOneRequest();

    std::vector<int> getUnspecifedFd();

private:
    fd_set socketSet;
    int maxfd;
    std::unordered_map<int, std::function<void(int)>> fdVec;
    std::function<void(int)> defaultAction;
};

class CstyleNetServer {
public:

    CstyleNetServer(int port);

    void send(const std::string &hostname, int port, std::string info);

    std::string receive();

private:
    std::string buffer;
    IOMultiplexingUtility io;
    Server server;
    Client c;
};

