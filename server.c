// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include <proto/dos.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    char *hello = "Hello from server";

   BPTR output = 0;
          
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s\n",buffer );
        
        //execv(buffer, 0);
        
        /* AmigaOS specific */
        output = IDOS->Open("output.txt", MODE_NEWFILE);
        IDOS->SystemTags(buffer, SYS_Output, output ? output : 0, TAG_DONE);
        if (output) IDOS->Close(output);
        /* -- */
        
		FILE *fh = fopen("output.txt", "r");
		char string[4096];
		int len = 4096; 
        do {
        	char *result = fgets(string, len, fh);
            if(result) write(new_socket , string, strlen(string));
            else break;
        } while(1);
        fclose(fh);
            
        printf("------------------Hello message sent-------------------\n");
        close(new_socket);
    }
    return 0;
}
