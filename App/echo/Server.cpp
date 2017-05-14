#include <NetService.hpp>
#include <thread>
#include <easylogging++.h>

using namespace std;

void startServe(short connfd) {
    std::string buf;
    while (rioRead(connfd, buf), !buf.empty()) {
        LOG(DEBUG) << "Recv >>" << buf << "<< from fd " << connfd;
        transform(buf.begin(), buf.end(), buf.begin(), ::toupper);
        rioWrite(connfd, buf);
        buf.clear();
    }
}

int main(int argc, char *argv[]) {
    initLog(argc, argv);
    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);
    Server s{port};
    try {
        while (true)
            std::thread(startServe, s.waitConnection()).detach();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}