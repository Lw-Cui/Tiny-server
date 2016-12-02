#include <NetService.hpp>
#include <easylogging++.h>

using namespace std;

int main(int argc, char *argv[]) {
    START_EASYLOGGINGPP(argc, argv);
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::ToFile, "false");
    el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format,
                                       "[%logger] %msg [%fbase:%line]");

    unsigned short port = 2000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);
    try {
        NetService se("localhost", port);
        std::string buf;
        while (cin >> buf) {
            se.writeStr(buf).readStr(buf);
            cout << buf << endl;
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}