#include "CSNode.hpp"
#include "Strings.hpp"
bool CSNode::doBind (int port) {
    //if (hasBinding) return true;

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

    //set reuseaddr
    int j;
    if(setsockopt(bindSocket, SOL_SOCKET, SO_REUSEADDR, &j, sizeof(int)) < 0 ) {
			perror("setsockopt");
            close(bindSocket);
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

CSNode::CSConnection *CSNode::waitForIncomming(int port) {
    if(!hasBinding) {
        doBind(port);
    }
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

    return connection;
}

CSNode::CSConnection *CSNode::connectToPeer (const char *address, int port) {
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
    return connection;
}

void CSNode::closeConnection (CSNode::CSConnection *connection) {
    if (connection) {
        close (connection->connectionSocket);
        delete connection;
    }
}

string CSNode::readSentence (CSConnection *connection, char stopCharacter = '\3') { //ETX
    string result;
    const int Bufsize = 1024;
    int bytes; char buffer[1024];

    while ((bytes = recv(connection->connectionSocket, buffer, Bufsize, 0)) >= 0) {
        connection->readBuffer.fill(buffer, bytes);
        if (connection->readBuffer.contains ('\3'))
            break;
    }
    if (bytes < 0) {
        perror ("recv)");
        exit (0);
    }
    return connection->readBuffer.read();
}

bool CSNode::writeSentence (CSConnection *connection, string sentence) {
    Buffer writeBuffer;
    writeBuffer.fill ((char *)sentence.c_str(), sentence.length());
    writeBuffer.fill ("\3\0", 2);
    string out = writeBuffer.read('\0');
    int bytes = send (connection->connectionSocket, out.c_str(), out.length(), 0);
    if (bytes == sentence.length()) return true;
    return false;
}

void CSNode::createServer (CSConnection *connection) {
    int pid = fork();

    if(pid == -1) {
        perror("fork");
        return;
    } else if (pid == 0) {
        while (1) {
            string message;
            try {
                message = readSentence (connection);
            } catch (exception dummy) {
                perror ("recv");
                exit(0);
            }

            astream a(message);
            vector<string> argv = a.split(' ');
            string keyword = argv[0];

            if (!keyword.compare("MESSAGE")) {
                string output = "<message> : ";
                for (int i = 1; i < argv.size(); i++) {
                    output += argv[i];
                    if (i < argv.size() - 1) output += " ";
                }
                cout << output << "\n";
            } else if (!keyword.compare("CLOSE")) {
                // if remote node is a server, help to close
                writeSentence(connection, "CLOSE");
                closeConnection (connection);
                exit(0); // abandon...
            } else if (!keyword.compare("PUSH")) {

            } else if (!keyword.compare("PULL")) {

            }
            printf("> "); //reinsert the prompt
        }
    } else {
        return;
    }
}