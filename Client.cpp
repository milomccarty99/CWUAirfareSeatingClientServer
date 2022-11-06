#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <errno.h>
#define PORT 5437

using namespace std;

int main(int argc, char * argv[])
{
    int sock =0, valread, client_fd;
    struct sockaddr_in serv_addr;
    string msg = "hello from client";
    //char *message = msg.c_str() ;
    char buffer[1024];
    sock = socket(AF_INET, SOCK_STREAM,0);
    if(sock < 0)
    {
        cout<< "socket creation error" << endl;
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); // feed the port number to htons

    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0)
    {
        cout << "Invalid address\address not supported" << endl;
        return -1;
    }
    client_fd = connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(client_fd < 0)
    {
        cout << "Connection failed"<< endl;
        return -1;
    }
    send(sock,msg.c_str(),sizeof(msg.c_str()),0);
    cout<<"hello message sent" << endl;
    valread = read(sock,buffer,1024);

    cout << buffer << endl;
    close(client_fd);
    return 0;
}