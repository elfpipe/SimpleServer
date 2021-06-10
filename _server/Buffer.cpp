#ifndef BUFFER_HPP
#define BUFFER_HPP
#include <string>
#include <memory.h>
using namespace std;
class Buffer {
private:
    char *buffer;
    int size;

public:
    Buffer () {
        size = 0; buffer = 0;
    }
    void fill (char *content, int contentSize) {
        char *newBuffer = (char *) malloc (size + contentSize);
        if (buffer) memcpy (newBuffer, buffer, size);
        memcpy (newBuffer + size, content, contentSize);
        size += contentSize;
        if (buffer) free (buffer);
        buffer = newBuffer;
    }
    string read (char limiter = '\3') {
        if (!buffer) return string();
        int limit = -1;
        for (int i = 0; i < size; i++) {
            if (buffer[i] == limiter) {
                limit = i;
                break;
            }
        }
        char readBuffer[limit + 1];
        memcpy (readBuffer, buffer, limit);
        readBuffer[limit] = '\0';
        cut (limit + 1);
        return string(readBuffer);
    }
    void cut (int amount) {
        char *newBuffer = (char *) malloc (size - amount);
        memcpy (newBuffer, buffer + amount, size - amount);
        size -= amount;
        free (buffer);
        buffer = newBuffer;
    }
    bool contains (const char character = '\3') {
        for (int i = 0; i < size; i++)
            if (buffer[i] == character) return true;
        return false;
    }
};
#endif //BUFFER_HPP

// #include <iostream>
// int main() {
//     Buffer buffer;
//     char *text1 = "Hej med dig";
//     buffer.fill (text1, strlen(text1));
//     buffer.fill ("\3", 1);
//     char *text2 = "Det var da dejligt, du lige kunne kigge \3 forbi.";
//     char *text3 = "\3 hutteli-hut";
//     buffer.fill (text2, strlen(text2));
//     buffer.fill (text3, strlen(text3));

//     cout << "buffer : " << buffer.read('\3') << "\n";
//     cout << "buffer : " << buffer.read('\3') << "\n";
//     cout << "buffer : " << buffer.read('\3') << "\n";

//     return 0;
// }
