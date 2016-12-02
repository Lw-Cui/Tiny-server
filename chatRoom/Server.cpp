#include <NetService.hpp>
#include <thread>
#include <easylogging++.h>
#include <RWLock.hpp>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);
    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);
    return 0;
}

