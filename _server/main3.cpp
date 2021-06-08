#include "CSNode.hpp"
#include <iostream>
#include "Strings.hpp"

bool term = false;

CSNode::CSConnection *do_command(CSNode &node, string command, CSNode::CSConnection *connection = 0) {
    astream a(command);
    string stripped = a.get('\n');
    a.setString(stripped);
    vector<string> argv = a.split();
    string keyword = argv[0];

    if(!keyword.compare("SERVE")) {
        if(argv.size() < 2) {
            cout << "Usage : SERVE <port>\n";
        } else {
            cout << "--- Waiting for incomming ---\n";
            CSNode::CSConnection *newConnection = node.waitForIncomming();

            if (newConnection) {
                cout << "Successfully received incomming ---*\n";
                if (connection) {
                    cout << "Closing previous connection (closed).\n";
                    node.closeConnection (connection);
                }
                connection = newConnection;
            } else {
                cout << "Failed to receive incomming |\n";
            }
        }
        return connection;
    } else if(!keyword.compare("CONNECT")) {
        if(argv.size() < 3) {
            cout << "Usage : CONNECT <address> <port>\n";
        } else {
            CSNode::CSConnection *newConnection = node.connectToPeer(argv1.c_str(), atoi(argv2.c_str()));

            if (newConnection) {
                cout << "Successfully connected to " << argv1 << "\n";
                if (connection) {
                    cout << "Closing previous connection (closed).\n";
                    node.closeConnection (connection);
                }
                connection = newConnection;
            } else {
                cout << "Failed to connect to " <<  argv1 << ":" << argv2 << "\n";
            }
        }
        return connection;
    } else if (!keyword.compare("CLOSE")) {
        node.closeConnection(connection);
        cout << "Connection closed\n";
        connection = 0;
    } else if (!keyword.compare("QUIT")) {
        term = true;
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
    } while (!term);

    if (connection) {
        node.closeConnection (connection);
    }
    return 0;
}