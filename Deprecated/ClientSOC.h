//
// Created by white-inside on 7/4/24.
//

#ifndef CLIENTSOC_H
#define CLIENTSOC_H
#include <string>


class ClientSOC {
    int port {};
    public:
        void run(char *buffer,long buffer_size);
        ClientSOC();
        std::string serialize_key(char key[]);
};



#endif //CLIENTSOC_H
