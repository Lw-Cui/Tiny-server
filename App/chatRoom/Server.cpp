#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);
    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    IOMultiplexingUtility io;
    Server server(port);
    io.addFd(server.Listening(), [&io, &server](int)mutable -> void {
        io.addFd(server.waitConnection(), nullptr);
        LOG(DEBUG) << "Add new client";
    });

    io.setDefaultAction([&io](int fd)mutable -> void {
        string str;
        rioRead(fd, str);
        auto client = io.getUnspecifedFd();
        for (auto c: client) rioWrite(c, str);
        LOG(DEBUG) << "Notify >>" << str << "<<";
    });

    while (true) io.start();
    return 0;
}

