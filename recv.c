#include <stdio.h>
#include <sys/socket.h>

char *safe_recv(int socket) {
    static char buffer[4096];
    int bytes, offset = 0;
    do {
        bytes = recv(socket, buffer + offset, sizeof(buffer) - 1 - offset, 0);
        offset += bytes;
    } while (bytes > 0 && buffer[offset - 1] != '\3' && buffer[offset - 1] != '\4'); //ETX
    if (bytes < 0)
	perror("recv");
    return buffer;
}

