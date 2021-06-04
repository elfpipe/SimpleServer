//client/server transaction from samme node

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/file.h>

#include <vector>
#include <string>

using namespace std;

class CSNode {
private:
    int bindSocket, port;
    bool hasBinding;

public:
    struct CSConnection {
        int connectionSocket;
        string identityString;
    };

    vector<CSConnection *> openConnections;    

public:
    bool doBind (int port);
    void unBind ();
    CSConnection *waitForIncomming();
    CSConnection *connectToPeer (char *address, int port);
    void closeConnection (CSConnection *connection);
};
