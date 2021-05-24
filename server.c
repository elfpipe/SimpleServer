// Server side C program
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8021

int term = 0;

char *safe_recv(int socket) {
    static char buffer[4096];
    int bytes, offset = 0;
    do {
	bytes = recv (socket, buffer + offset, sizeof(buffer) - 1 - offset, 0);
	offset += bytes;
    } while (bytes > 0
	&& buffer[offset - 1] != '\3'
	&& buffer[offset - 1] != '\4');

    if (buffer[offset - 1] == '\4')
        term = 1;
    buffer[offset - 1] = '\0';
//    if (bytes < 0)
//	perror("recv");
    return buffer;
}

int main(int argc, char const *argv[])
{
    int sock, connect;

    /* Server addess */
    struct sockaddr_in address = (struct sockaddr_in){  
        AF_INET,
        htons(PORT),
        (struct in_addr){INADDR_ANY}
    };
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
        
    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if (listen(sock, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
	term = 0;
        if ((connect = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("CONNECT from 0x(%x) \n", address.sin_addr.s_addr);
        
	while(!term) {
            char *message = safe_recv (connect);
	    if(strlen(message)) {
		printf("MESSAGE : %s\n", message);

       	        send(connect, "<revc>", strlen("<recv>"), 0);
	        send(connect, "\3", 1, 0);
	    }
	}
        printf("..<terminate connection>--\n");
//	sleep(1);
        close(connect);
    }
    close(sock);
    return 0;
}
