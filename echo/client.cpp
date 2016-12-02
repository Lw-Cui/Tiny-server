#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    initLog(argc, argv);

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);
    try {
        Client c{"localhost", port};
        std::string buf;
        while (getline(cin, buf)) {
            if (!c.writeStr(buf).readStr(buf)) break;
            cout << buf << endl;
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}