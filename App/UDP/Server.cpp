#include <NetService.hpp>
#include <thread>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);
    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);
    Server s;
    auto listenfd = s.Listening(port, UDP);
    while (true) {
        // For datagram sockets, this call returns the entire datagram that was sent,
        // provided that the datagram fits into the specified buffer.
        // Stream sockets act like streams of information with no boundaries separating data.
        // For example, if applications A and B are connected with a stream socket and application A sends 1000 bytes,
        // each call to this function can return 1 byte, or 10 bytes, or the entire 1000 bytes.
        // Therefore, applications using stream sockets should place this call in a loop,
        // calling this function until all data has been received.
        struct sockaddr_in addrClient;
        socklen_t addrLen = sizeof(struct sockaddr_in);
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        auto nMsgLen = recvfrom(listenfd, buf, 1023, 0, (struct sockaddr *) &addrClient, &addrLen);
        LOG(DEBUG) << "read message >>" << buf << "<< and length is: " << nMsgLen;

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(buf + nMsgLen, " (Time: %d-%d-%d %d:%d:%d)", tm.tm_year + 1900,
                tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        std::string msg{buf};
        sendto(listenfd, msg.c_str(), msg.size(), 0, (struct sockaddr *) &addrClient, addrLen);
    }
    return 0;
}
