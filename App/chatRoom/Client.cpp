#include <NetService.hpp>
#include <log.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    Client c;
    IOMultiplexingUtility io;

    io.addFd(fileno(stdin), [&c](int)mutable -> void {
        string str;
        cin >> str;
        NetReadWrite::rioWrite(c.getConnfd(), str);
    });
    io.addFd(c.connectServer("localhost", port), [&c](int fd)mutable -> void {
        string str;
        if (!NetReadWrite::rioRead(c.getConnfd(), str)) err_sys("Read EOF");
        cout << "Read notify >>" << str << "<<" << endl;
    });
    while (true) io.processOneRequest();
    return 0;
}
