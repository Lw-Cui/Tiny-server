#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    Client c;
    IOMultiplexingUtility io;

    io.addFd(fileno(stdin), [&c](int)mutable -> void {
        char tmp[100];
        fgets(tmp, 100, stdin);
        tmp[strlen(tmp) - 1] = 0;
        c.writeStr(tmp);
    });
    io.addFd(c.connectServer("localhost", 2000), [&c](int fd)mutable -> void {
        string str;
        if (!c.readStr(str)) err_sys("Read EOF");
        printf("Read notify >>%s<<\n", str.c_str());
        fflush(stdout);
    });
    while (true) io.start();
    return 0;
}
