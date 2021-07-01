#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NAME_LEN 50

// structs
struct Request {
    int len;
    char username[NAME_LEN];
};

// global variables
int sockfd, clientfd, pid, x;
int addrlen = sizeof(struct sockaddr_in);
struct sockaddr_in server, client;

// main function
int main(int argc, char *argv[]) {
    
    // get command line argument
    if(argc != 2) {
        perror("Usage: ./fingerserver <port #>\n");
        return -1;
    }
    int port = atoi(argv[1]);
    
    // create socket
    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket\n");
        exit(1);
    }
    
    // bind server to port
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Cannot bind to port\n");
        exit(1);
    }
    
    // listen for connections
    if(listen(sockfd, 3) < 0) {
        perror("Cannot listen for connections\n");
        exit(1);
    }
    
    // wait for new client connections
    while(1) {
    
        // accept connection
        if((clientfd = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&addrlen)) < 0) {
            perror("Cannot accept connection\n");
            exit(1);
        }
        
        // fork process
        if((pid = fork()) < 0) {
            perror("Cannot fork\n");
            exit(1);
        }
        
        // process clients
        if(!pid) {
            
            // stop listening for more clients in child process
            if(close(sockfd) < 0) {
                perror("Cannot close socket\n");
                exit(1);
            }
            
            // receive username from client
            struct Request msg;
            int bytes_rcvd = 0;
            void *p = (void *)&msg;
            while(bytes_rcvd < sizeof(msg)) {
                x = read(clientfd, p, sizeof(msg)-bytes_rcvd);
                if(x <= 0) break;
                p += x;
                bytes_rcvd += x;
            }
            msg.len = ntohl(msg.len);
            if(x == -1) {
                perror("Error receiving username from client\n");
                close(clientfd);
                exit(1);
            }
            
            // execute finger program
            char *args[3];
            args[0] = "finger";
            args[1] = (char *)&msg.username[0];
            args[2] = (char *)NULL;
            if(dup2(clientfd, 1) < 0) {
                perror("Pipe failed\n");
                exit(1);
            }
            if(dup2(clientfd, 2) < 0) {
                perror("Pipe failed\n");
                exit(1);
            }
            if(close(clientfd) < 0) {
                perror("Cannot close socket\n");
                exit(1);
            }
            if(execvp(args[0], args)) {
                perror("Child process failed\n");
                exit(1);
            }
            
        } else {
            
            // close socket for current client in parent process
            if(close(clientfd) < 0) {
                perror("Cannot close socket\n");
                exit(1);
            }
            
        }
           
    }

    return 0;
    
}