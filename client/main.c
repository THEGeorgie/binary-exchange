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

#include "httpImp.h"
#include "global-variables.h"

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
    char *buf = malloc(MAXDATASIZE);
    if (buf == NULL) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }
    char *response = malloc(MAXDATASIZE);
    if (response == NULL) {
        perror("Memory allocation failed");
        return EXIT_FAILURE;
    }
    size_t bytesRead;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];


    if (argc != 4) {
        fprintf(stderr,"usage: client hostname, directory of program,  directory of data(must be a csv file)\n");
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

    buf = create_request("POST", "/login", "","");
    printf("client: sending '%s'\n",buf);

    if (send(sockfd, buf, strlen(buf), 0) == -1) {
        perror("send");
    }

    if ((numbytes = recv(sockfd, response, MAXDATASIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    response[numbytes] = '\0';

    char *msg = strstr(response, "\r\n\r\n");
    if (msg == NULL) {
        perror("client: ");
    }
    json_object * jObj = json_tokener_parse(msg);
    extract_js_packet_int(jObj, "id", &id);
    printf("client: received id %d\n", id);
    json_object_put(jObj);

    printf("Program name: %s, data name: %s", argv[2], argv[3]);
    buf = create_request("POST", "/process", argv[3],argv[2]);
    printf("client: sending process request: '%s'\n",buf);

    if (send(sockfd, buf, strlen(buf), 0) == -1) {
        perror("send");
    }

    close(sockfd);
    free(buf);
    free(response);


    return 0;
}