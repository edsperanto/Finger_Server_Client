#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define NAME_LEN 50
#define RESULT_LEN 2000

// structs
struct Request {
    int len;
    char username[NAME_LEN];
};

// global variables
int sockfd, rv, n;
struct addrinfo hints, *servinfo, *p;

// main function
int main(int argc, char *argv[]) {
    
    // get command line argument and extract username and hostname
    if(argc != 2) {
        perror("Usage: ./fingerclient username@hostname:port\n");
        return -1;
    }
    char *saveptr, *username, *hostnameport, *hostname, *port;
    username = strtok_r(argv[1], "@", &saveptr);
    hostnameport = strtok_r(NULL, "@", &saveptr);
    hostname = strtok_r(hostnameport, ":", &saveptr);
    port = strtok_r(NULL, ":", &saveptr);
    
    // get host address information
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
        perror("Cannot get address information\n");
        exit(1);
    }
    
    // loop through all results
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        // establish socket connection
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            perror("Cannot create socket\n");
            exit(1);
        }
        
        // connect to server
        if(connect(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("Cannot connect to server\n");
            close(sockfd);
            continue;
        }
        
        // send username to server
        struct Request msg;
        strcpy(msg.username, username);
        msg.len = htonl((int)strlen(username));
        void *p = (void *)&msg;
        int bytes_sent = 0;
        while(bytes_sent < sizeof(msg)) {
            n = write(sockfd, p, sizeof(msg)-bytes_sent);
            if(n == -1) {
                perror("Error sending message to server\n");
                close(sockfd);
                exit(1);
            }
            p += n;
            bytes_sent += n;
        }
        
        // receive and print response from server
        int bytes_received = 0;
        char server_reply[RESULT_LEN];
        while((n = recv(sockfd, &server_reply[bytes_received], sizeof(server_reply)-n, 0)) > 0)
            bytes_received += n;
        if(n == -1) {
            perror("Error receiving response from server\n");
            exit(1);
        }
        printf("%s\n", server_reply);
        
        // connection successful, exit program
        if(close(sockfd) < 0) {
            perror("Cannot close socket\n");
            exit(1);
        }
        break;
        
    }

    return 0;
    
}