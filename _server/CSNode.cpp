#include "CSNode.hpp"

bool CSNode::doBind (int port) {
    if (hasBinding) return true;

    // server address
    this->port = port;
    struct sockaddr_in address = (struct sockaddr_in) {
        AF_INET,
        htons(port),
        (struct in_addr){INADDR_ANY}
    };
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((bindSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket");
        return false;
    }

    //do binding to INADDR_ANY
    if (bind (bindSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close (bindSocket);
        return false;
    }

    //listen
    if (listen(bindSocket, 10) < 0) {
        close (bindSocket);
        perror("listen");
        return false;
    }
    hasBinding = true;
    return true;
}

void CSNode::unBind () {
    if (hasBinding) close (bindSocket);
    hasBinding = false;
}

CSNode::CSConnection *CSNode::waitForIncomming() {    
    CSConnection *connection = new CSConnection;

    struct sockaddr_in address = (struct sockaddr_in) {
        AF_INET,
        htons(port),
        (struct in_addr){INADDR_ANY}
    };
    int addrlen = sizeof(address);
    
    if ((connection->connectionSocket = accept(bindSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        return 0;
    }

    //convert incomming address to presentation text
    char addressBuffer[1024];
    if (inet_ntop (AF_INET, &address.sin_addr, addressBuffer, sizeof(addressBuffer)) > 0) {
        connection->identityString = addressBuffer;
    }

    openConnections.push_back(connection);
    return connection;
}

CSNode::CSConnection *CSNode::connectToPeer (char *address, int port) {
    CSConnection *connection = new CSConnection;
    if ((connection->connectionSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
    	return 0;
    }

    struct sockaddr_in saddr;
    memset(&saddr, '0', sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);

    bool success = true;
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, address, &saddr.sin_addr) <= 0) {
	    printf("<inet_pton> : Invalid address /or Address not supported \n");
        success = false;
    }

    if (connect (connection->connectionSocket, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("connect");
        success = false;
    }

    if (!success) {
        close (connection->connectionSocket);
        delete connection;
        return 0;
    }

    connection->identityString = address;
    openConnections.push_back (connection);

    return connection;
}

void CSNode::closeConnection (CSNode::CSConnection *connection) {
    if (connection) {
        close (connection->connectionSocket);

        for(int i = 0; i < openConnections.size(); i++) {
            if(openConnections.at(i) == connection) openConnections.erase (openConnections.begin() + i);
        }
        delete connection;
    }
}