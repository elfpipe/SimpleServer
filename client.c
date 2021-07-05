// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/file.h>

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

int safe_send(int socket, char *message)
{
    int bytes = send (socket, message, strlen(message), 0);
    send (socket, "\3", 1, 0);
    return bytes;
}

#define MIN(x, y) ((x)>(y) ? (y) : (x))

int do_PUSH (int sock, char *filename)
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

int do_PULL (int sock, char *filename) // PUSH from client
{
    //read size from connect
    char *sizestr = safe_recv(sock);
    int size = atoi(sizestr);

    if (size == 0) {
    	printf("<PULL> : file size 0, abort\n");
    	return -1;
    }

    printf("<read file> : %s , size %d\n", filename, size);

    //read file to disk
    int fd = open (filename, O_CREAT|O_WRONLY|O_TRUNC); //read, write and execute permission
    if (fd < 0) {
	    perror("open");
	    return -1;
    }
    // int rbytes = 0;
    // while (rbytes < size) {
    //     char buffer[4096*46];
	//     int len = recv (sock, buffer, sizeof(buffer), 0);
	//     if (len < 0) return -1;
	//     write (fd, buffer, len);
	//     rbytes += len;
    // }

    char *buffer = (char *)malloc (size+1);
    printf("Calling recv...\n");
    int len = recv (sock, buffer, size, 0);
    printf("Return.\n");
    write(fd, buffer, len);

    printf("<PULL> : success\n");

    close (fd);
    if(len != size)
    	printf("Warning : PUSH returned file of Odd size\n");

    return 0;
}

char *commands[] = {
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
	if(!strcmp(commands[i], command)) {
	    c = i;
	    break;
	}
    }
    switch (c) {
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
    //write out response from server
    // puts(safe_recv(sock));
    return 0;
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

//    puts(safe_recv(sock));

    do {
        char command[1024] = "";

        printf("> ");
        fgets(command, 1023, stdin);
   	    command[strlen(command) - 1] = '\0'; //strip '\n'

        if(!strcmp(command, "QUIT")) {
	    send(sock, "\4", 1, 0);
            break;
	    }

    	safe_send (sock, command);
    	do_command(sock, command);
    } while (!term);

    close(sock);

    return 0;
}
