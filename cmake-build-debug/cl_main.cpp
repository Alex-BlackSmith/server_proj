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
#include <vector>
#include <map>
#include <istream>
#include "TwoDimArray.h"
#include <libtcod.hpp>
using std::ifstream;
using std::istream;
using std::string;
using std::vector;
using std::map;


#define PORT "50001" // the port client will be connecting to
#define MAXBUFFERSIZE 10000

//istream buffer;
char buf[MAXBUFFERSIZE];

void PaintOfScreen(TwoDimArray<char>& TwoDArray, const vector<TCODColor>& colVec, vector<int>& tmpPlrPos, map<vector<int>,char>& mapOfWinPositions){
    TCODConsole::initRoot(TwoDArray.getDimX(),TwoDArray.getDimY(),"AlexSmith's Sokoban", false, TCOD_RENDERER_GLSL);
    TCODConsole::root->setDefaultBackground({0,0,0});

    for (auto i = 0; i < TwoDArray.getDimY(); i++){
        for (auto j = 0; j < TwoDArray.getDimX(); j++){
            TCODConsole::root -> setChar(j, i, TwoDArray.getObjPos(i,j));
            if (TwoDArray.getObjPos(i,j) == 'P'){
                TCODConsole::root->setCharBackground(j,i,colVec[0]);
            }
            else if (TwoDArray.getObjPos(i,j) == '#'){
                TCODConsole::root->setCharBackground(j,i,colVec[1]);
            }
            else if (TwoDArray.getObjPos(i,j) == 'B'){
                TCODConsole::root->setCharBackground(j,i,colVec[2]);
            }
            else if (TwoDArray.getObjPos(i,j) == 'X'){
                TCODConsole::root->setCharBackground(j,i,colVec[3]);
            }
        }
    }

    /*for (auto i = 0; i < TwoDArray.getDimY(); i++){
        for (auto j = 0; j < TwoDArray.getDimX(); j++){

            if (TCODConsole::root -> getChar(j, i) == 'P'){
                TCODConsole::root->setCharBackground(j,i,colVec[0]);
            }
            else if (TCODConsole::root -> getChar(j, i) == '#'){
                TCODConsole::root->setCharBackground(j,i,colVec[1]);
            }
            else if (TCODConsole::root -> getChar(j, i) == 'B'){
                TCODConsole::root->setCharBackground(j,i,colVec[2]);
            }
            else if (TCODConsole::root -> getChar(j, i) == 'X'){
                TCODConsole::root->setCharBackground(j,i,colVec[3]);
            }
        }
    }*/
    TCODConsole::root->flush();
}


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
    const TCODColor player {0,255,0};
    const TCODColor wall{255,0,0};
    const TCODColor box{255,255,0};
    const TCODColor winCross{255,255,255};
    const vector<TCODColor> colourVec = {player, wall, box, winCross}; //0 - player, 1 - wall, 2 - box, 3 - winCross
    vector<int> tempPlrPos; //temporary player position vector (x,y)
    map<vector<int>,char> mapCharWin;


    TwoDimArray<char> Test; //create instance of 2DArray class
    bool isMapRecieved = false;
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
    if ((numbytes = recv(socket_desc, buf, MAXBUFFERSIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    if (ch == 'm' && !isMapRecieved) {
        membuf str_buf(buf, buf + sizeof(buf));
        std::istream in(&str_buf);
        in >> Test;
        //std::cout << Test;
        PaintOfScreen(Test, colourVec, tempPlrPos, mapCharWin);
        isMapRecieved = true;
    }

    close(socket_desc);
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