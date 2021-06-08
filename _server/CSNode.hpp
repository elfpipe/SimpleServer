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

    static const int Bufsize = 16;
    struct CSBuffer {
        char *buffer;
        int size = 0, offset = 0, max = 0;
        CSBuffer (int size = Bufsize) {
            buffer = (char *) malloc (size);
            this->size = Bufsize;
            offset = 0; max = 0;
        }
        int find (char sChar) {
            for (int i = offset; i < max; i++) {
                if (buffer[i] == sChar)
                    return i;
            }
            return -1;
        }
        void fill (char *content, int size) { //fills buffer and returns true if content has stopChar in it
            if (size < this->size - max) {
                memcpy (buffer + max, content, size);
                max += size;
            } else {
                int newSize = max - offset + size;
                char *newBuf = (char *) malloc (newSize);
                memcpy (newBuf, buffer + offset, max - offset);
                memcpy (newBuf + max - offset, content, size);
                max = newSize;
                free (buffer);
                buffer = newBuf;
                offset = 0;
                this->size = newSize;
            }
        }
        void flush () {
            if (offset == size - 1 || offset == max) {
                //flush entire buffer
                offset = 0; max = 0;
                free (buffer); buffer = (char *) malloc (Bufsize); size = Bufsize;
            } else if (offset > 0) {
                //flush only first and last part
                char *newBuf = (char *) malloc (max - offset);
                memcpy (newBuf, buffer + offset, max - offset);
                free (buffer); buffer = newBuf;
                size = max - offset; offset = 0; max -= offset;
            }
        }
    };

public:
    struct CSConnection {
        int connectionSocket;
        string identityString;
        CSBuffer readBuffer;
    };

    vector<CSConnection *> openConnections;    

public:
    bool doBind (int port);
    void unBind ();
    CSConnection *waitForIncomming();
    CSConnection *connectToPeer (char *address, int port);
    void closeConnection (CSConnection *connection);

    string readSentence (CSConnection *connection, char stopCharacter = '\3') { //ETX
        string result;
        char buffer[Bufsize];
        int bytes;

        while ((bytes = recv(connection->connectionSocket, buffer, Bufsize, 0)) > 0) {
            connection->readBuffer.fill(buffer, bytes);
            int charAt = connection->readBuffer.find (stopCharacter);
            if (charAt != -1) {
                char resultBuffer[charAt - connection->readBuffer.offset];
                memcpy (resultBuffer,
                    connection->readBuffer.buffer + connection->readBuffer.offset,
                    charAt - connection->readBuffer.offset);
                result += resultBuffer; break;
            }
        }
        connection->readBuffer.flush(); //clean up buffer heads
        return result;
    }

    bool writeSentence (CSConnection *connection, string sentence) {
        int bytes = send (connection->connectionSocket,
            sentence.c_str(), sentence.length(), 0);
        send (connection->connectionSocket, "\3", 1, 0);
        if (bytes == sentence.length()) return true;
        return false;
    }
};
