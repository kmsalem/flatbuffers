#include <iostream>
#include <utility>
#include "obj_api_generated.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <random>
#include <chrono>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5050

using namespace Comparison::Experiment;

/*
    This is experiment transfering flatbuffer through Socket
        - data is built locally first and then packed into flatbuffer before being sent
        - data is unpacked into native c++ structure after being received 
        - socket code is copied from online: https://www.geeksforgeeks.org/socket-programming-cc/
*/
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./api_socket_experiment path_to_config id num_entries container_size(in bytes)" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    int num_entries = atoi(argv[3]);
    int size = atoi(argv[4]);

    int64_t key = 0;
    flatbuffers::FlatBufferBuilder builder;

    char *buffer = new char[size];

    struct MainT *m;
    struct MainT *n;
    if (id == 0) {
        // setup object#1
        m = new MainT();
        std::string str = "";
        while(str.size() != 10) {
            str.append("a");
        }
        // build the data
        while (key < num_entries) {
            m->testVector2.push_back(str);
            key++;
        }

        // setup connection
        int server_fd, new_socket, valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        
        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        
        // Forcefully attaching socket to the port 5050
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );
        
        // Forcefully attaching socket to the port 5050
        if (bind(server_fd, (struct sockaddr *)&address, 
                                    sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // printf("start the experiment...\n");
        auto start = std::chrono::high_resolution_clock::now();

        auto main1 = Main::Pack(builder, m);
        builder.Finish(main1);
        size = builder.GetSize();
        
        send(new_socket, &size, sizeof(int), 0);
        valread = send(new_socket, builder.GetBufferPointer(), size, 0);
        // size = builder.GetSize();
        // char *sbuff = (char *)builder.GetBufferPointer();
        // while (valread < size) {
        //     int sent = send(new_socket, sbuff+valread, sizeof(char) * (size-valread), 0);  // error here
        //     if (sent <= 0) {
        //         printf("current sent is %d\n", sent);
        //         break; // error/close connection
        //     }
        //     valread += sent;
        // }
        
        // printf("Byte sent is %d\n", valread);
        
        valread = 0;
        recv(new_socket, &size, sizeof(int), 0);
        while (valread < size) {
            //int now = read(sock, buffer, size);
            int read = recv(new_socket, buffer+valread, sizeof(char) * (size-valread), 0);
            if (read == -1) {
                printf("\n recv failed \n");
                break;
            }
            if (read == 0) break;
            valread += read;
        }
        // printf("Byte received is %d\n", valread);
        n = GetMain(buffer)->UnPack();
        // std::cout << n->testVector2[0] << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        delete m;
        close(new_socket);
    } else {
        // setup object#2
        n = new MainT();
        std::string str = "";
        while(str.size() != 10) {
            str.append("b");
        }
        // build the data
        while (key < num_entries) {
            n->testVector2.push_back(str);
            key++;
        }

        // setup connection
        //struct sockaddr_in address;
        int sock = 0, valread;
        struct sockaddr_in serv_addr;
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)  // create a tcp_socket
        {
            printf("\n Socket creation error \n");
            return -1;
        }
    
        memset(&serv_addr, '0', sizeof(serv_addr));
    
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        
        // Convert IPv4 and IPv6 addresses from text to binary form
        if(inet_pton(AF_INET, "10.70.0.9", &serv_addr.sin_addr)<=0) 
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }
    
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }

        // printf("Ready for the experiment...\n");

        valread = 0;
        recv(sock, &size, sizeof(int), 0);
        // printf("Byte received should be received is %d\n", size);
        while (valread < size) {
            //int now = read(sock, buffer, size);
            int read = recv(sock, buffer+valread, sizeof(char) * (size-valread), 0);
            if (read < 0) {
                printf("current read is %d\n", read);
                printf("\n recv failed \n");
                break;
            }
            if (read == 0) break;
            valread += read;
        }

        // printf("Byte received is %d\n", valread);

        m = GetMain(buffer)->UnPack();
        // std::cout << m->testVector2[0] << std::endl;

        auto main2 = Main::Pack(builder, n);  // call Pack here will cause bad_alloc() error
        builder.Finish(main2);
        size = builder.GetSize();
        // std::cout << "Buffer size is " << size << std::endl;
        send(sock, &size, sizeof(int), 0);
        valread = send(sock, builder.GetBufferPointer(), size, 0);
        // printf("Byte sent is %d\n", valread);
        
        delete n;
        close(sock);
    }

    delete[] buffer;
    return 0;
}