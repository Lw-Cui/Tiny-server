#include <NetService.hpp>
#include <log.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);
    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    IOMultiplexingUtility io;
    Server server;
    io.addFd(server.Listening(port), [&io, &server](int)mutable -> void {
        io.addFd(server.waitConnection(), nullptr);
        LOG(DEBUG) << "Add new client";
    });

    io.setDefaultAction([&io](int fd)mutable -> void {
        string str;
        NetReadWrite::rioRead(fd, str);
        auto client = io.getUnspecifedFd();
        for (auto c: client) NetReadWrite::rioWrite(c, str);
        LOG(DEBUG) << "Notify >>" << str << "<<";
    });

    while (true) io.processOneRequest();
    return 0;
}

