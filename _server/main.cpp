#include "CSNode.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage : %s <address> <port>\n", argv[0]);
        return 0;
    }

    CSNode node;
    CSNode::CSConnection *connection = node.connectToPeer(argv[1], atoi(argv[2]));

    if (connection) {
        printf("Successfully connected to %s\n", argv[1]);
    } else {
        printf("Failed to connect to %s\n", argv[1]);
    }

    node.closeConnection(connection);
    printf("Connection closed\n");

    return 0;    
}