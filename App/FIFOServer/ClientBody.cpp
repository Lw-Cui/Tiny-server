#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <NetService.hpp>
#include <log.h>

using namespace std;
#define SERVER "Server"

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    auto wfd = open(SERVER, O_WRONLY);
    auto rfd = open(CLIENT, O_RDONLY);

    IOMultiplexingUtility io;
    io.addFd(fileno(stdin), [&io, wfd](int)mutable -> void {
        string str;
        cin >> str;
        NetReadWrite::rioWrite(wfd, str);
    });

    io.addFd(rfd, [&io](int rfd)mutable -> void {
        string str;
        NetReadWrite::rioRead(rfd, str);
        LOG(DEBUG) << "Read >>" << str << "<<";
    });

    while (true) io.processOneRequest();
    return 0;
}


