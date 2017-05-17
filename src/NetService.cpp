#include <NetService.hpp>
#include <errno.h>
#include <string.h>

using namespace std;

INITIALIZE_EASYLOGGINGPP

int Client::connectServer(const std::string &hostname, int port) {
    if ((connfd = socket(AF_INET, type, 0)) < 0)
        err_sys("Open socket error");

    struct hostent *hostInfo = nullptr;
    if ((hostInfo = gethostbyname(hostname.c_str())) == nullptr)
        err_sys("Get hostname error");

    struct sockaddr_in serverSocket;
    memset(reinterpret_cast<char *>(&serverSocket), 0, sizeof(serverSocket));

    serverSocket.sin_family = AF_INET;
    bcopy(hostInfo->h_addr_list[0], reinterpret_cast<char *>(&serverSocket.sin_addr.s_addr),
          static_cast<size_t>(hostInfo->h_length));
    serverSocket.sin_port = htons(port);

    if (connect(connfd, reinterpret_cast<struct sockaddr *>(&serverSocket), sizeof(serverSocket)) < 0)
        err_sys("connect error");
    LOG(DEBUG) << "Connect server.";
    return connfd;
}

void rioRead(int fd, std::string &usrbuf) {
    while (true) {
        char c;
        long int rc{read(fd, &c, 1)};
        // IMPORTANT!
        if (rc == 1 && c != '\0') { usrbuf.push_back(c); }
        if (rc == 0 || c == '\0') return; // EOF
        if (rc < 0) err_sys("Read failed");
    }
}

void rioWrite(int fd, const string &usrbuf) {
    size_t nleft{usrbuf.length()};
    ssize_t nwritten;
    const char *bufp{usrbuf.c_str()};
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) nwritten = 0;
            else err_sys("Write failed");
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    write(fd, "\0", 1); // write EOF
}

void initLog(int argc, char **argv) {
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%logger] %msg [%fbase:%line]");
}


int Server::Listening() {
    if ((listenfd = socket(AF_INET, type, 0)) < 0)
        err_sys("Socket open failed");

    int optval = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const void *>(&optval), sizeof(int)) < 0)
        err_sys("Setsockopt failed");

    struct sockaddr_in clientSocket;
    memset(&clientSocket, 0, sizeof(clientSocket));
    clientSocket.sin_family = AF_INET;
    clientSocket.sin_addr.s_addr = htons(INADDR_ANY);
    clientSocket.sin_port = htons(port);

    if (::bind(listenfd, reinterpret_cast<sockaddr *>(&clientSocket), sizeof(clientSocket)) < 0)
        err_sys("Bind socket failed");

    if (type == TCP && listen(listenfd, LISTENQ) < 0)
        err_sys("Listen port failed");

    return listenfd;
}

int Server::waitConnection() {
    if (listenfd == -1) Listening();
    if (type == UDP)
        err_sys("UDP doesn't need accept connection");
    struct sockaddr_in clientSocket;
    unsigned int clientlen = sizeof(clientSocket);
    int connfd = -1;
    if ((connfd = accept(listenfd, reinterpret_cast<sockaddr *>(&clientSocket), &clientlen)) < 0)
        err_sys("Accept failed");
    return connfd;
}

void err_sys(const char *fmt, ...) {
    constexpr int MAXLINE = 500;
    char buf[MAXLINE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, MAXLINE, fmt, ap);
    va_end(ap);
    snprintf(buf + strlen(buf), MAXLINE - strlen(buf), ": %s", strerror(errno));
    strcat(buf, "\n");
    fflush(stdout);
    fputs(buf, stderr);
    fflush(NULL);
    exit(1);
}

void IOMultiplexingUtility::start() {
    auto readySet = socketSet;
    select(maxfd + 1, &readySet, NULL, NULL, NULL);
    for (auto &pair: fdVec)
        if (FD_ISSET(pair.first, &readySet)) {
            if (pair.second != nullptr)pair.second(pair.first);
            else if (defaultAction) defaultAction(pair.first);
        }
}

std::vector<int> IOMultiplexingUtility::getUnspecifedFd() {
    std::vector<int> vec;
    for (auto &pair: fdVec)
        if (pair.second == nullptr)
            vec.push_back(pair.first);
    return vec;
}
