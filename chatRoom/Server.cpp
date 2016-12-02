#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

void notifyAll(IOMultiplexingServer &io, const std::string &str) {
    LOG(DEBUG) << "Notify to all client >>" << str << "<<";
    auto client = io.getClient();
    for (auto c: client) rioWrite(c, str);
}

int main(int argc, char *argv[]) {
    initLog(argc, argv);
    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    try {
        IOMultiplexingServer io{port};
        while (true) {
            auto client = io.getReadyClient();
            for (auto c: client) {
                string str;
                rioRead(c, str);
                notifyAll(io, str);
            }
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

