#include "CSNode.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    CSNode node;
    if (node.doBind(8021)) {
        CSNode::CSConnection *connection = node.waitForIncomming();

        if (connection) {
            printf("Successfully connected to %s\n", argv[1]);
        } else {
            printf("Failed to connect to %s\n", argv[1]);
            return -1;
        }

        string message = "Hello! How are you?";
        cout << "Writing message to upstream : " << message;
        node.writeSentence (connection, message);

        node.closeConnection(connection);
        printf("Connection closed\n");
    }
    return 0;    
}