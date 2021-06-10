#include "Buffer.cpp"
#include "CSNode.hpp"
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <strings.h>
class Server {
private:
    CSNode *node;
    Buffer readBuffer;
    int exitPipe[2];
public:
    Server (CSNode *node) {
        this->node = node;
    }
    void startThread () {
        pthread_t threadHandle;
        pipe(exitPipe);
        pthread_create (&threadHandle, 0, thread, (void *)this);

    }
    static void *thread(void *dummy) {
        Server *_this = (Server *) dummy;
        struct pollfd pfds[2];
        pfds[0] = {_this->node->bindSocket, POLLIN, 0};
        pfds[1] = {_this->exitPipe[0], POLLIN, 0};

        while (1) {
            if (poll (pfds, 2, 100) < 0) {
                perror("poll");
                pthread_exit(0);
            }
            
            if(pfds[0].revents & POLLIN) {
                cout << "Icomming call...\n";

                CSNode::CSConnection *connection = _this->node->waitForIncomming (_this->node->port);
                if (connection) {
                    cout << "<message> : " << _this->node->readSentence (connection, '\3') << "\n";
                    _this->node->writeSentence (connection, "CLOSE");
                    _this->node->closeConnection (connection);
                    cout << "Call completed.\n";
                }
            }
            if(pfds[1].revents & POLLIN) {
                cout << "exitPipe activatet (EXIT)\n";
                pthread_exit(0);
            }
        }
    }
    void endThread() {
        write (exitPipe[1], "hej\0", 4);
    }
};


int main() {
    CSNode node;

    node.doBind (8022);

    Server server (&node);
    server.startThread ();

    printf("Thread started.\n");

    char dummy[1024];
    fgets (dummy, 1024, stdin);

    node.unBind();

    server.endThread();
    pthread_exit(0);
}