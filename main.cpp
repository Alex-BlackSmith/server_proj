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
#include <map>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT1 "50001"
#include "TwoDimArray.h"
#include <fstream>
#include <ostream>
#include <vector>
#include <libtcod.hpp>

using std::ifstream;
using std::string;
using std::vector;
using std::map;
#define BACKLOG 10   // how many pending connections queue will hold
#define MAXBUFFERSIZE 602

void chkKeyKeyAndMovePlayer(const char& key ,vector<int>& tmpPlrPos, TwoDimArray<char>& twoDimArray);

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
{   /*TwoDimArray<char> Test;
    const TCODColor player {0,255,0};
    const TCODColor wall{255,0,0};
    const TCODColor box{255,255,0};
    const TCODColor winCross{255,255,255};
    const vector<TCODColor> colourVec = {player, wall, box, winCross}; //0 - player, 1 - wall, 2 - box, 3 - winCross
    vector<int> tempPlrPos; //temporary player position vector (x,y)
    map<vector<int>,char> mapCharWin; // vector of win positions "(x,y) - character"
    */

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
    char address_pres[INET6_ADDRSTRLEN];
    bool isMapSent = false;
    bool *isMapSentPtr = &isMapSent;

    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT1, &hints, &servinfo1)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

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
                  address_pres, sizeof address_pres);
        //printf("server: got connection from %s\n", s);
        if (!fork()) { // this is the child process
            close(sock); // child doesn't need the listener

            //close(new_sock);
            if ((numbytes = recv(new_sock, buf, MAXBUFFERSIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            if (*buf == 'm' && !isMapSent) {
                if (send(new_sock, buffer, size, 0) == -1){
                    perror("send");
                }
                else {
                    *isMapSentPtr = true;
                }
            }
            if(((*buf == 'w') || (*buf == 'a')
            || (*buf == 's') || (*buf == 'd'))) {
                //chkKeyKeyAndMovePlayer(*buf, tempPlrPos, Test);

                printf("server: received '%s'\n", buf);

            }
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


/*void chkKeyKeyAndMovePlayer(const char& key ,vector<int>& tmpPlrPos, TwoDimArray<char>& twoDimArray){
    //TCOD_key_t key = TCODConsole::checkForKeypress();
    //TCODConsole::root->flush();
    if ( key == 'w' || key == 'W' ) {
        if (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] - 1) != '#'){
            if (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] - 1) != 'B'){
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1] - 1, 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1] - 1,colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                tmpPlrPos = {tmpPlrPos[0], tmpPlrPos[1] - 1};
            }
            else if ((twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] - 1) == 'B')
                     && (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] - 2) != '#')
                     && (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] - 2) != 'B')){
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1] - 1, 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1] - 1,colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1] - 2, 'B');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1] - 2,colVec[2]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                tmpPlrPos = {tmpPlrPos[0], tmpPlrPos[1] - 1};
            }
        }
        TCODConsole::root->flush();
    }
    else if ( key == 'a' || key == 'A' ) {
        if (twoDimArray.getObjPos(tmpPlrPos[0] - 1, tmpPlrPos[1]) != '#'){
            if (twoDimArray.getObjPos(tmpPlrPos[0] - 1, tmpPlrPos[1]) != 'B'){
                twoDimArray.setObjPos(tmpPlrPos[0] - 1, tmpPlrPos[1], 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0] - 1, tmpPlrPos[1],colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                tmpPlrPos = {tmpPlrPos[0] - 1, tmpPlrPos[1]};
            }

            else if ((twoDimArray.getObjPos(tmpPlrPos[0] - 1, tmpPlrPos[1]) == 'B')
                     && (twoDimArray.getObjPos(tmpPlrPos[0] - 2, tmpPlrPos[1]) != '#')
                     && (twoDimArray.getObjPos(tmpPlrPos[0] - 2, tmpPlrPos[1]) != 'B')){
                twoDimArray.setObjPos(tmpPlrPos[0] - 1, tmpPlrPos[1], 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0] - 1, tmpPlrPos[1],colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                twoDimArray.setObjPos(tmpPlrPos[0] - 2, tmpPlrPos[1], 'B');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0] - 2, tmpPlrPos[1],colVec[2]);
                tmpPlrPos = {tmpPlrPos[0] - 1, tmpPlrPos[1]};
            }
        }
        TCODConsole::root->flush();
    }
    else if ( key == 's' || key == 'S' ) {
        if (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] + 1) != '#'){
            if (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] + 1) != 'B'){
                twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] + 1, 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1] + 1,colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                tmpPlrPos = {tmpPlrPos[0], tmpPlrPos[1] + 1};
            }
            else if ((twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] + 1) == 'B')
                     && (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] + 2) != '#')
                     && (twoDimArray.getObjPos(tmpPlrPos[0], tmpPlrPos[1] + 2) != 'B')){
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1] + 1, 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1] + 1,colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1] + 2, 'B');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1] + 2,colVec[2]);
                tmpPlrPos = {tmpPlrPos[0], tmpPlrPos[1] + 1};
            }
        }
        TCODConsole::root->flush();
    }
    else if ( key == 'd' || key == 'D' ) {
        if (twoDimArray.getObjPos(tmpPlrPos[0] + 1, tmpPlrPos[1]) != '#'){
            if (twoDimArray.getObjPos(tmpPlrPos[0] + 1, tmpPlrPos[1]) != 'B'){
                twoDimArray.getObjPos(tmpPlrPos[0] + 1, tmpPlrPos[1], 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0] + 1, tmpPlrPos[1],colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                tmpPlrPos = {tmpPlrPos[0] + 1, tmpPlrPos[1]};
            }

            else if ((twoDimArray.getObjPos(tmpPlrPos[0] + 1, tmpPlrPos[1]) == 'B')
                     && (twoDimArray.getObjPos(tmpPlrPos[0] + 2, tmpPlrPos[1]) != '#')
                     && (twoDimArray.getObjPos(tmpPlrPos[0] + 2, tmpPlrPos[1]) != 'B')){
                twoDimArray.setObjPos(tmpPlrPos[0] + 1, tmpPlrPos[1], 'P');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0] + 1, tmpPlrPos[1],colVec[0]);
                twoDimArray.setObjPos(tmpPlrPos[0], tmpPlrPos[1], ' ');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0], tmpPlrPos[1],{0,0,0});
                twoDimArray.setObjPos(tmpPlrPos[0] + 2, tmpPlrPos[1], 'B');
                //TCODConsole::root->setCharBackground(tmpPlrPos[0] + 2, tmpPlrPos[1],colVec[2]);

                tmpPlrPos = {tmpPlrPos[0] + 1, tmpPlrPos[1]};
            }
        }
    }
}

void firstFeel(TwoDimArray<char>& twoDArray, vector<int>& tmpPlrPos, map<vector<int>,char>& mapOfWinPositions){

    for (auto i = 0; i < twoDArray.getDimY(); i++){
        for (auto j = 0; j < twoDArray.getDimX(); j++){

            if (twoDArray.getObjPos(i,j) == 'P'){
                tmpPlrPos = {j,i};
            }
            else if (twoDArray.getObjPos(i,j) == '#'){

            }
            else if (twoDArray.getObjPos(i,j) == 'B'){


            }
            else if (twoDArray.getObjPos(i,j) == 'X'){
                mapOfWinPositions[{j,i}] = 'X';
            }
        }
    }
}*/