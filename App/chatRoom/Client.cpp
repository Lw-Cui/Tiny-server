#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    try {
        IOMultiplexingClient c{"localhost", port};
        c.loop([&](const std::string &str) {
                    cout << str << endl;
                });
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}
