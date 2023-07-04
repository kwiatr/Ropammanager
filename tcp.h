#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <unistd.h>

class TcpClient
{
    private:
        int sock;
        std::string address;
        int port;
        struct sockaddr_in server;

    public:
        TcpClient();
        bool ropamConnect(std::string address, int port);
        bool send_binary(std::vector<unsigned char> data);        
        std::vector<unsigned char> receive_binary(int size=512);
        bool disconnect();

};
