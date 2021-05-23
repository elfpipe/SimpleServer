// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8021

int main(int argc, char const *argv[])
{
    int sock, connect;
    int reuse = 1;
    long bytesread;

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

    /* Address can be reused instantly after program exits */
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof reuse);
        
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
        if ((connect = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("CONNECT from 0x(%x) \n", address.sin_addr.s_addr);
        
        char buffer[1024];
        while (bytesread = read(connect, buffer, sizeof(buffer))) {
            buffer[bytesread] = '\0';
            printf("Command: %s\n",buffer);
        }

        printf("..<terminate connection>--\n");
        close(connect);
    }
    close(sock);
    return 0;
}
