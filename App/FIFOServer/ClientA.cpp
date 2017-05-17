#include <stdio.h>
#include <fcntl.h>
#include <string>
#include <NetService.hpp>

using namespace std;
#define SERVER "Server"
#define CLIENT "ClientA"

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    auto wfd = open(CLIENT, O_WRONLY);
    auto rfd = open(SERVER, O_RDONLY);

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

    while (true) io.start();
    return 0;
}