#include "CSNode.hpp"
#include <iostream>
#include "Strings.hpp"

bool term = false;

CSNode::CSConnection *do_command(CSNode &node, string command, CSNode::CSConnection *connection = 0) {
    astream a(command);
    string stripped = a.get('\n');
    a.setString(stripped);
    vector<string> argv = a.split(' ');
    string keyword = argv[0];

    cout << "argv[0]: '" << argv[0] << "'\n";

    if(!keyword.compare("SERVE")) {
        if(argv.size() < 2) {
            cout << "Usage : SERVE <port>\n";
        } else {
            cout << "(*) Serving calls on port " << argv[1] << "....\n";

            CSNode::CSConnection *newConnection = node.waitForIncomming (atoi(argv[1].c_str()));
            if(newConnection) {
                cout << "Gracefully accepted call from " << newConnection->identityString << " :)\n";
                connection = newConnection;
                node.createServer (connection);
            }
        }
    } else if(!keyword.compare("CALL")) {
        if(argv.size() < 3) {
            cout << "Usage : CALL <address> <port>\n";
        } else {
            if (connection) {
                cout << "Please close previous connection to " << connection->identityString << " first. (CALL)\n";
                return connection;
            }

            CSNode::CSConnection *newConnection = node.connectToPeer(argv[1].c_str(), atoi(argv[2].c_str()));

            if (newConnection) {
                cout << "Successfully connected to " << argv[1] << "\n";
                cout << "Sending credentials to " << argv[1] << "\n";
                // -- bla bla --
                cout << "Host accepted call.\n";
                connection = newConnection;
                node.createServer (connection);
            } else cout << "Failed to connect to " << argv[1] << "\n";
        }
        return connection;
    } else if (!keyword.compare("MESSAGE")) {
        if(connection) {
            node.writeSentence (connection, stripped);
        } else cout << "No connection\n";
    } else if (!keyword.compare("CLOSE")) {
        if(connection) {
            node.writeSentence(connection, "CLOSE");
            node.closeConnection(connection);
            cout << "Connection closed\n";
            connection = 0;
        } else cout << "Not connected.\n";
    } else if (!keyword.compare("EXIT")) {
        if (connection) {
            node.writeSentence (connection, "CLOSE");
            node.closeConnection (connection);
        }
        cout << "Connection closed. Exit.\n";
        exit(0);
    } else if (!keyword.compare("PUSH")) {

    } else if (!keyword.compare("PULL")) {

    }
    return connection;
}

int main(int argc, char *argv[]) {
    CSNode node;
    CSNode::CSConnection *connection = 0;

    do {
        char command[1024] = "";

        printf("> ");
        fgets(command, 1023, stdin);
    	connection = do_command(node, command, connection);
    } while(true);

    //return 0;
}