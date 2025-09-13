/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>      // printf, perror
#include <sys/types.h>

#define PORT "3490"  // the port users will be connecting to
#define MAXDATASIZE 2097152 //2MB
#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s) {
    (void) s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

char *handle_request(char *data, size_t data_size) {
    char * response_line = "HTTP/1.1 200 OK\r\n";
    char * blank_line = "\r\n";
    char * response_body = "Request recived";
    char response[MAXDATASIZE];

    sprintf(response,sizeof(response), response_line, blank_line,response_body);

    printf("%s", response);
    return response;
}

typedef struct {
    sem_t mutex;
    char buf[MAXDATASIZE];
} shared_data;

int main(void) {
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct stat fileStat; //stores file status read,write,execute permissions
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    FILE *fp;
    int numbytes = 0;

    if (stat("server", &fileStat) == -1) {
        perror("stat");
        exit(1);
    }

    // Create or open a shared memory object
    int fd;
    if ((fd = shm_open("com_buf", O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory
    if (ftruncate(fd, sizeof(shared_data)) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory into the process's address space
    shared_data *data = mmap(NULL, sizeof(shared_data),
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize the semaphore in shared memory (1 for process-shared)
    if (sem_init(&data->mutex, 1, 1) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Initialize the buffer to an empty string
    data->buf[0] = '\0';

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    while (1) {
        // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("\nserver: got connection from %s\n", s);

        if (!fork()) {
            // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Connected", strlen("Connected"), 0) == -1) {
                perror("send");
            }
            char buf[MAXDATASIZE];
            int totalReceived = 0;
            int bytesReceived;

                /*
                while (totalReceived < MAXDATASIZE) {
                    bytesReceived = recv(new_fd, buf1 + totalReceived, MAXDATASIZE - totalReceived, 0);
                    if (bytesReceived == -1) {
                        perror("recv");
                        break;
                    } else if (bytesReceived == 0) {
                        break;  // connection closed
                    }
                    totalReceived += bytesReceived;
                }
                */
                if ((numbytes = recv(new_fd, buf, MAXDATASIZE, 0)) == -1) {
                    perror("recv");
                }
                buf[numbytes] = '\0';
                char *response = handle_request(buf, numbytes);
                printf("Sending:%s", response);
                if (send(new_fd, response, strlen(response), 0) == -1) {
                    perror("server: send");
                }
                /*
                FILE *out = fopen("programs/test.out", "wb");
                if (out) {
                    fwrite(data->buf, 1, totalReceived, out);
                    fclose(out);
                }
                sem_post(&data->mutex);

                fileStat.st_mode = S_ISUID;

                if (chmod("programs/test.out", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
                    perror("chmod");
                    exit(1);
                }

                if (execl("programs/test.out", "test.out", (char *)NULL) == -1) {
                    perror("execl");
                    exit(1);
                }

                if (unlink("programs/test.out") == -1) {
                    perror("unlink");
                    exit(1);
                }
                */
            }
            close(new_fd);
            exit(0);
    }

    close(sockfd);
    sem_destroy(&data->mutex);
    munmap(data, sizeof(shared_data));
    shm_unlink("/my_shm");
    return 0;
}