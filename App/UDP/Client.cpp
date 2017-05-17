/*
#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    Client c;
    c.connectServer("localhost", port, UDP);
    IOMultiplexingUtility io;

    io.addFd(c.getConnfd(), [&c](int)mutable -> void {
        char cbuf[1024];
        memset(cbuf, 0, sizeof(cbuf));
        read(c.getConnfd(), cbuf, 1024);
        cout << cbuf << endl;
    });

    io.addFd(fileno(stdin), [&c](int)mutable -> void {
        string buf;
        getline(cin, buf);
        write(c.getConnfd(), buf.c_str(), buf.size());
    });

    while (true) io.start();
    return 0;
}
 */
#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);
    try {
        Client c;
        c.connectServer("localhost", port, UDP);
        std::string buf;
        char cbuf[1024];
        while (getline(cin, buf)) {
            write(c.getConnfd(), buf.c_str(), buf.size());
            memset(cbuf, 0, sizeof(cbuf));
            read(c.getConnfd(), cbuf, 1024);
            LOG(DEBUG) << "read message: " << cbuf;
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
