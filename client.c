// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int term = 0;

char *safe_recv(int socket) {
    static char buffer[4096];
    int bytes, offset = 0;
    do {
	bytes = recv(socket, buffer + offset, sizeof(buffer) - 1 - offset, 0);
	offset += bytes;
    } while (bytes > 0 && buffer[offset-1] != '\3' && buffer[offset-1] != '\4'); //ETX || EOT
    buffer[offset - 1] = '\0';
    if (buffer[offset - 1] == '\4')
        term = 1;
    if(bytes < 0)
	perror("recv");
    return buffer;
}

int main(int argc, char const *argv[])
{
    int sock = 0;
    struct sockaddr_in address;

    if (argc < 3) {
    	printf("USAGE: %s <ip> <port>\n", argv[0]);
    	return 0;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
	printf("\n Socket creation error \n");
	return -1;
    }

    memset(&address, '0', sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, argv[1], &address.sin_addr) <= 0)
    {
	printf("\nInvalid address/ Address not supported \n");
	return -1;
    }

    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("connect");
        return -1;
    }

    do {
        char command[1024];

        printf("> ");
        fgets(command, 1023, stdin);
	command[strlen(command) - 1] = '\0'; //strip '\n'

        if(!strcmp(command, "QUIT")) {
	    send(sock, "\4", 1, 0);
            break;
	}

        send(sock, command, strlen(command) ,0);
	send(sock, "\3", 1, 0);
	
	puts(safe_recv(sock));
    } while (!term);

    close(sock);

    return 0;
}
