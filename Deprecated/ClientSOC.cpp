//
// Created by white-inside on 7/4/24.
//

#include "ClientSOC.h"

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void ClientSOC::run(char *buffer, long buffer_size) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("ERROR CREATE SOCKET FOR SERVER");
    }
    struct sockaddr_in server_socket_struct;
    // server_socket_struct.sin_addr.s_addr = INADDR_LOOPBACK;
    server_socket_struct.sin_port = htons(port);
    server_socket_struct.sin_family = AF_INET;
    inet_aton("127.0.0.1", &server_socket_struct.sin_addr);

    int status = connect(
        client_socket, (sockaddr *)&server_socket_struct,
        sizeof(server_socket_struct));
    if (status < 0) {
        perror("ERROR CONNECT TO SOCKET");
    }
    std::cout << "CONNECTED!" << std::endl;
    char hostname[250];
    gethostname(hostname, sizeof(hostname));
    char username[250];
    getlogin_r(username, sizeof(username));

    std::string message{"{"};
    std::string hostnamevalue= "\""+static_cast<std::string>(hostname)+"\"";
    message += serialize_key("machine") + ":" + serialize_key(hostname) ;
    message +=",";
    message += serialize_key("user") + ":" + serialize_key(username) ;
    message += "}";
    send(client_socket, message.data(), message.size(), 0);
    char response[8048];
    recv(client_socket, response, 2048,0);

    std::cout << "DATA SENT!"<<std::endl << std::endl;
    std::cout << response;
    close(client_socket);

}

ClientSOC::ClientSOC()
    : port {8081}
{

}

std::string ClientSOC::serialize_key(char key[]) {
    std::string res{"\""};
    res+=key;
    res+="\"";
    return res;
}