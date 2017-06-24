#include <NetService.hpp>

using namespace std;

int main(int argc, char *argv[]) {

    unsigned short port = 10000;
    if (argc == 2) sscanf(argv[1], "%hu", &port);

    CstyleNetServer server(port);
    cout << server.receive() << endl;
    server.send("localhost", 20000, "hello peerA");
    cout << server.receive() << endl;
    server.send("localhost", 20000, "bye!");
    server.send("localhost", 20000, "see you next time!");
    return 0;
}

