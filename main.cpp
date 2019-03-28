#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT1 "50001"
#include "TwoDimArray.h"
#include <fstream>
#include <ostream>
#include <vector>

using std::ifstream;
using std::string;

#define BACKLOG 10   // how many pending connections queue will hold
#define MAXBUFFERSIZE 602

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// processing appropriate ip4 or ip6 address
void *get_approp_addr(struct sockaddr *sock_a);

int main(void)
{
    ifstream in("TestMap.txt");
    std::ifstream file("TestMap.txt", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    char buffer[size];
    file.read(buffer, size);



    char buf[MAXBUFFERSIZE];
    int sock, new_sock,numbytes;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo1, *servinfo2, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];

    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, PORT1, &hints, &servinfo1)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    /*if ((rv = getaddrinfo(NULL, PORT2, &hints, &servinfo2)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }*/

    // loop through all the results and bind to the first we can
    for(p = servinfo1; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("server: bind");
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo1); // all done with this structure
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if (listen(sock, BACKLOG) == -1) {
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
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_sock = accept(sock, (struct sockaddr *)&their_addr, &sin_size);
        if (new_sock == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
                  get_approp_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);
        //printf("server: got connection from %s\n", s);
        if (!fork()) { // this is the child process
            close(sock); // child doesn't need the listener

            //close(new_sock);
            if ((numbytes = recv(new_sock, buf, MAXBUFFERSIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            if (send(new_sock, buffer, size, 0) == -1)
                perror("send");
            printf("server: received '%s'\n",buf);
            exit(0);
        }
        close(new_sock);  // parent doesn't need this
    }
    return 0;
}

void *get_approp_addr(struct sockaddr *sock_a)
{
    if (sock_a->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sock_a)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sock_a)->sin6_addr);
}