/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 2097152 //2MB

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    size_t bytesRead;
    char msg[300];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    FILE *fp;

    if (argc != 3) {
        fprintf(stderr,"usage: client hostname, directory of program\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
                }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, msg, (sizeof(msg)/sizeof(msg[0])), 0)) == -1) {
        perror("recv");
        exit(1);
    }
    msg[numbytes] = '\0';
    printf("client: received '%s'\n",msg);

    fp = fopen(argv[2], "r");
    if (!fp) {
        perror("fopen failed");
        exit(1);
    }

    size_t totalBytesRead = 0;
    while ((bytesRead = fread(buf + totalBytesRead, 1, MAXDATASIZE - totalBytesRead, fp)) > 0) {
        totalBytesRead += bytesRead;
        if (totalBytesRead >= MAXDATASIZE) {
            break;
        }
    }
    printf("client: sending '%s'\n",buf);
    fclose(fp);

    if (send(sockfd, buf, totalBytesRead, 0) == -1) {
        perror("send");
    }
    //test

    close(sockfd);

    return 0;
}