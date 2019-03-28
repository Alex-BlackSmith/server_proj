#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fstream>
#include <ostream>
#include <istream>
#include "TwoDimArray.h"
using std::ifstream;
using std::istream;
using std::string;


#define PORT "50001" // the port client will be connecting to
#define MAXBUFFERSIZE 10000

//istream buffer;
char buf[MAXBUFFERSIZE];

// get appropriate sockaddress, IPv4 or IPv6:
void *get_approp_addr(struct sockaddr *sock_a);

struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};

int main(int argc, char *argv[])
{
    TwoDimArray<char> Test; //create instance of 2DArray class

    int socket_desc, numbytes;

    struct addrinfo hints, *list_of_server_info, *p;
    int getaddrinfo_code;
    char address_pres[INET6_ADDRSTRLEN],ch;
    /*if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }*/
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    while(std::cin >> ch){
    if ((getaddrinfo_code = getaddrinfo("127.0.0.1", PORT, &hints, &list_of_server_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_code));
        return 1;
    }
    // loop through all the results and connect to the first we can

    for(p = list_of_server_info; p != NULL; p = p->ai_next) {
        if ((socket_desc = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(socket_desc, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_desc);
            perror("client: connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_approp_addr((struct sockaddr *)p->ai_addr),
              address_pres, sizeof address_pres);
    //printf("client: connecting to %s\n", address_pres);
    freeaddrinfo(list_of_server_info); // all done with this structure
    if (send(socket_desc, &ch, 1, 0) == -1)
        perror("send");
    //close(socket_desc);
    //close(sockfd);
    if ((numbytes = recv(socket_desc, buf, MAXBUFFERSIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    //std::stringstream ss(std::string(buf));
    membuf str_buf(buf, buf + sizeof(buf));

    std::istream in(&str_buf);
    in >> Test;
    std::cout << Test;
    close(socket_desc);
    //buf[numbytes] = '\0';
    //printf("client: received from server echo'%s'\n",buf);
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