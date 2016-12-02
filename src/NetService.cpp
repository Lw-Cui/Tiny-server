#include <NetService.hpp>
#include <cstdio>

using namespace std;

INITIALIZE_EASYLOGGINGPP

void Client::connectServer(const std::string &hostname, int port) {
    LOG(DEBUG) << "Opening socket descriptor.";
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw server_error("Open socket error.");

    struct hostent *hostInfo = nullptr;
    LOG(DEBUG) << "Parsing hostname " << hostname;
    if ((hostInfo = gethostbyname(hostname.c_str())) == nullptr)
        throw server_error("Get hostname error.");

    struct sockaddr_in serverSocket;
    memset(reinterpret_cast<char *>(&serverSocket), 0, sizeof(serverSocket));

    serverSocket.sin_family = AF_INET;
    bcopy(hostInfo->h_addr_list[0], reinterpret_cast<char *>(&serverSocket.sin_addr.s_addr),
          static_cast<size_t>(hostInfo->h_length));
    serverSocket.sin_port = htons(port);

    LOG(DEBUG) << "Trying to connect server...";
    if (connect(connfd, reinterpret_cast<struct sockaddr *>(&serverSocket), sizeof(serverSocket)) < 0)
        throw server_error("connect error.");
    LOG(DEBUG) << "Connect server.";
}

void rioRead(int fd, std::string &usrbuf) {
    while (true) {
        char c;
        long int rc{read(fd, &c, 1)};
        // IMPORTANT!
        if (rc == 1 && c != '\0') { usrbuf.push_back(c); }
        if (rc == 0 || c == '\0') return; // EOF
        if (rc < 0) throw server_error("Read failed.");
    }
}

void rioWrite(int fd, const string &usrbuf) {
    size_t nleft{usrbuf.length()};
    ssize_t nwritten;
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

void initLog(int argc, char **argv) {
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%logger] %msg [%fbase:%line]");
}


int Server::Listening() {
    LOG(DEBUG) << "Open socket.";
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw server_error("Socket open failed.");

    LOG(DEBUG) << "Manipulate options.";
    int optval = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   reinterpret_cast<const void *>(&optval), sizeof(int)) < 0)
        throw server_error("Setsockopt failed.");

    struct sockaddr_in clientSocket;
    memset(&clientSocket, 0, sizeof(clientSocket));
    clientSocket.sin_family = AF_INET;
    clientSocket.sin_addr.s_addr = htons(INADDR_ANY);
    LOG(DEBUG) << "Start listening port " << port;
    clientSocket.sin_port = htons(port);

    LOG(DEBUG) << "Trying to bind client socket into file descriptor";
    if (::bind(listenfd, reinterpret_cast<sockaddr *>(&clientSocket), sizeof(clientSocket)) < 0)
        throw server_error("Bind socket failed.");

    LOG(DEBUG) << "Start listening at port " << port;
    if (listen(listenfd, LISTENQ) < 0)
        throw server_error("Listen port failed.");

    return listenfd;
}

int Server::waitConnection() {
    if (listenfd == -1) Listening();
    struct sockaddr_in clientSocket;
    unsigned int clientlen = sizeof(clientSocket);
    int connfd = -1;
    if ((connfd = accept(listenfd, reinterpret_cast<sockaddr *>(&clientSocket), &clientlen)) < 0)
        throw server_error("Accept failed.");
    LOG(DEBUG) << "Accept connection(file descriptor): " << connfd;
    return connfd;
}

std::vector<int> IOMultiplexingServer::getReadyClient() {
    std::vector<int> readyVec;
    auto readySet = socketSet;
    select(maxfd + 1, &readySet, NULL, NULL, NULL);
    if (FD_ISSET(listenfd, &readySet))
        addClient(waitConnection());
    for (auto fd: clientVec)
        if (FD_ISSET(fd, &readySet))
            readyVec.push_back(fd);
    return std::move(readyVec);
}

void IOMultiplexingClient::loop(std::function<void(const std::string &)> callback) {
    while (true) {
        auto readySet = socketSet;
        select(connfd + 1, &readySet, NULL, NULL, NULL);
        if (FD_ISSET(0, &readySet)) {
            char tmp[100];
            fgets(tmp, 100, stdin);
            tmp[strlen(tmp) - 1] = 0;
            writeStr(tmp);
            LOG(DEBUG) << "Read stdin >>" << tmp << "<<";
        }
        if (FD_ISSET(connfd, &readySet)) {
            std::string str;
            if (!readStr(str)) {
                LOG(DEBUG) << "Read EOF";
                break;
            }
            callback(str);
            LOG(DEBUG) << "Read notify >>" << str << "<<";
        }
    }
}
