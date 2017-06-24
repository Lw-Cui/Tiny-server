#include <NetService.hpp>
#include <easylogging++.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
#define CLIENT_A "ClientA"
#define CLIENT_B "ClientB"
#define SERVER "Server"

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unlink(CLIENT_A);
    unlink(CLIENT_B);
    unlink(SERVER);

    mkfifo(CLIENT_A, 0744);
    mkfifo(CLIENT_B, 0744);
    mkfifo(SERVER, 0744);

    auto rfd = open(SERVER, O_RDWR);

    IOMultiplexingUtility io;
    io.addFd(open(CLIENT_A, O_WRONLY), nullptr);
    io.addFd(open(CLIENT_B, O_WRONLY), nullptr);

    io.addFd(rfd, [&io, rfd](int)mutable -> void {
        string str;
        NetReadWrite::rioRead(rfd, str);
        LOG(DEBUG) << "Read >>" << str << "<<";
        auto client = io.getUnspecifedFd();
        for (auto c: client) NetReadWrite::rioWrite(c, str);
    });

    while (true) io.processOneRequest();
    return 0;
}

