// Server side C program
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/file.h>

#define PORT 8022

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

int safe_send(int socket, char *message)
{
    int bytes = send (socket, message, strlen(message), 0);
    send (socket, "\3", 1, 0);
    return bytes;
}

int do_PUSH (int sock, char *filename) // PUSH from client
{
    //read size from connect
    char *sizestr = safe_recv(sock);
    int size = atoi(sizestr);

    if (size == 0) {
    	printf("<PUSH> : file size 0, abort\n");
    	return -1;
    }

    printf("<read file> : %s , size %d\n", filename, size);

    //read file to disk
    int fd = open (filename, O_CREAT|O_WRONLY|O_TRUNC); //read, write and execute permission
    if (fd < 0) {
	    perror("open");
	    return -1;
    }
    int rbytes = 0;
    while (rbytes < size) {
        char buffer[4096];
	    int len = recv (sock, buffer, sizeof(buffer), 0);
	    if (len < 0) return -1;
	    write (fd, buffer, len);
	    rbytes += len;
    }

    printf("<PUSH> : success\n");

    close (fd);
    if(rbytes != size)
    	printf("Warning : PUSH returned file of Odd size\n");

    return 0;
}

int do_PULL (int sock, char *filename)
{
    int fd = open (filename,  O_RDONLY);
    if (fd < 0) {
        safe_send(sock, "0"); //send file size 0
	    perror ("open");
	    return -1;
    }

    //calculate file size
    int size = lseek (fd, 0, SEEK_END);
    lseek (fd, 0, SEEK_SET);
    char sizebuf[128];

    //send file size as string
    sprintf(sizebuf, "%d", size);
    safe_send(sock, sizebuf);

    printf("<send file> : %s , size=%s\n", filename, sizebuf);

    //send file data
    int sent = 0;
    while (sent < size) {
        char buffer[4096];
        int len = read (fd, buffer, sizeof(buffer));
        if (len < 0) return -1;
    	send (sock, buffer, len, 0);
    	sent += len;
    }

    printf("<PUSH> : file sent\n");

    close (fd);
    return 0;
}

const char *commands[] = {
    "PUSH",
    "PULL",
    "CLOSE"
};

enum command_no {
    C_PUSH,
    C_PULL,
    C_CLOSE,
    C_COMMANDS
};

int do_command (int sock, char *message)
{
    char command[1024], argument[1024];
    sscanf (message, "%s %s", command, argument);

    int c = -1;
    for (int i = 0; i < C_COMMANDS; i++) {
        if (!strcmp(command, commands[i])) {
	    c = i;
	    break;
	}
    }
    switch(c) {
	case C_PUSH:
	    printf("<PUSH> : %s\n", argument);
	    do_PUSH (sock, argument);
	    break;
	case C_PULL:
	    printf("<PULL> : %s\n", argument);
        do_PULL (sock, argument);
	    break;
	case C_CLOSE:
	    printf("<CLOSE>\n");
	    break;
	default:
	    printf("<unknown command>\n");
	    break;
    }
    send(sock, "<revc>", strlen("<recv>"), 0);
    send(sock, "\3", 1, 0);
    return 0;
}

int main(int argc, char const *argv[])
{
    int sock, connect;

    /* Server address */
    struct sockaddr_in address = (struct sockaddr_in){  
        AF_INET,
        htons((sa_family_t)PORT),
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

        //send welcome message

        // const char *welcome = "Hello and welcome\n We are pleased to \3 tell you, that you are connected.\n\n";
        // send (connect, welcome, strlen (welcome), 0);


	while(!term) {
            char *message = safe_recv (connect);
	    printf("<command> : %s \n", message);

	    do_command(connect, message);
	}
        printf("..<terminate connection>--\n");
        close(connect);
    }
    close(sock);
    return 0;
}
