#include <NetService.hpp>
#include <thread>
#include <chrono>

using namespace std;

int main(int argc, char *argv[]) {

    unsigned short port = 20000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    CstyleNetServer server(port);
    server.send("localhost", 10000, "hello PeerB");
    cout << server.receive() << endl;
    server.send("localhost", 10000, "goodbye PeerB");
    cout << server.receive() << endl;
    cout << server.receive() << endl;

    for (int i = 0; i < 100000; i++) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        server.send("localhost", 10000, to_string(i));
    }
    return 0;
}
