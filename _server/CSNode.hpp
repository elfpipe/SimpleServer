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
#include <iostream>

#include "Buffer.cpp"

using namespace std;

class CSNode {
private:
public:
    int bindSocket, port;
    bool hasBinding;

public:
    struct CSConnection {
        int connectionSocket;
        string identityString;
        Buffer readBuffer;
    };

public:
    bool doBind (int port);
    void unBind ();

public:
    CSNode () { hasBinding = false; }

    CSConnection *waitForIncomming(int port);
    CSConnection *connectToPeer (const char *address, int port);
    void closeConnection (CSConnection *connection);

    string readSentence (CSConnection *connection, char stopCharacter = '\3');
    bool writeSentence (CSConnection *connection, string sentence);

    void createServer (CSConnection *connection);
};
