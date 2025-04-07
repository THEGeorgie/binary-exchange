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
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
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

    int shmid;
    key_t key = IPC_PRIVATE; // Use a unique key

    // Create the shared memory segment
    size_t shared_size = MAXDATASIZE ;
    if ((shmid = shmget(key, shared_size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if (stat("server", &fileStat) == -1) {
        perror("stat");
        exit(1);
    }


    // Attach the shared memory segment to the process's address space
    void *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    char *buf = (char *) shmaddr;


    sem_t mutex;
    if (sem_init(&mutex, 0, 1) != 0) {
        perror("sem_init failed");
        exit(1);
    }


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
        printf("server: got connection from %s\n", s);

        if (!fork()) {
            // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, "Connected", strlen("Connected"), 0) == -1)
                perror("send");
            char buf1[MAXDATASIZE];
            while (1) {

                if ((numbytes = recv(new_fd, buf1, MAXDATASIZE - 1, 0)) == -1) {
                    perror("recv");
                } else if (numbytes == 0) {
                    printf("server: connection closed\n");
                    for (int i = 0; i < BACKLOG; i++) {
                            break;
                    }
                    break;
                }

                sem_wait(&mutex);
                memcpy(buf, buf1, numbytes);
                fp = fopen("programs/test", "wb");
                fwrite(buf, 1, numbytes, fp);
                fclose(fp);
                fileStat.st_mode = S_ISUID;
                if (chmod("programs/test", fileStat.st_mode) == -1) {
                    perror("chmod");
                    exit(1);
                }
                if (execl("/home/gogo/CLionProjects/binary-exchange/server/cmake-build-debug/programs/test", "./test") == -1) {
                    perror("execl");
                    exit(1);
                }

                sem_post(&mutex);



            }
            close(new_fd);
            exit(0);
        }
    }

    close(sockfd);
    shmdt(shmaddr);
    shmctl(shmid, IPC_RMID, NULL);
    sem_destroy(&mutex);
    return 0;
}