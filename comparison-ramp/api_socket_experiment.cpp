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
    This is experiment transfering flatbuffer through Socket (not working right now...)
        - data is built using flatbuffers' object API and is packed before transfer
        - unpack() is not called; instead, Get*() can get the root table (not native table) directly
        - socket code is copied from online: https://www.geeksforgeeks.org/socket-programming-cc/
          1. connetion does not work sometimes...
          2. data transfer is not successful...
*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./api_socket_experiment path_to_config id container_size(in bytes)" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    size_t size = (size_t)atoi(argv[3]);

    int64_t key = 0;
    int num_entries = (size*0.60)/(88);

    flatbuffers::FlatBufferBuilder mbuilder;
    flatbuffers::FlatBufferBuilder nbuilder;

    char *buffer = new char(sizeof(char) * size);

    struct MainT m;
    struct MainT n;
    if (id == 0) {
        // setup object#1
        std::string str = "";
        while(str.size() != 32) {
            str.append("a");
        }
        // build the data
        while (key < num_entries) {
            m.testVector2.push_back(str);
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
        
        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );
        
        // Forcefully attaching socket to the port 8080
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

        printf("start the experiment...\n");
        auto start = std::chrono::high_resolution_clock::now();
        auto main1 = Main::Pack(mbuilder, &m);
        mbuilder.Finish(main1);
        //send(new_socket, mbuilder.GetBufferPointer(), mbuilder.GetSize(), 0);
        auto main_temp = GetMain(mbuilder.GetBufferPointer());
        std::cout << main_temp->testVector2()->Get(0)->str() << std::endl;
        std::cout << "Buffer size is " << mbuilder.GetSize() << " and total size is " << sizeof(char)*size << endl;
        send(new_socket, mbuilder.GetBufferPointer(), sizeof(char) * size, 0);

        // valread = read(new_socket, buffer, size);
        // auto main2 = GetMain(buffer);
        // std::cout << main2->testVector2->Get(0)->str() << std::endl;
        auto end = std::chrono::high_resolution_clock::now();
        printf("time cost is %f \n", (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    } else {
        // setup object#2
        std::string str = "";
        while(str.size() != 32) {
            str.append("b");
        }
        // build the data
        while (key < num_entries) {
            n.testVector2.push_back(str);
            key++;
        }

        // setup connection
        //struct sockaddr_in address;
        int sock = 0, valread;
        struct sockaddr_in serv_addr;
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

        printf("Ready for the experiment...\n");
        valread = read(sock, buffer, size);  // error here

        //valread = read(sock, buffer, sizeof(char) * size);

        // flatbuffers::Verifier verifier((uint8_t *)buffer, sizeof(char)*size);
        // bool verified = VerifyMainBuffer(verifier);

        // if (!verified) {
        //     printf("Error verifying flatbuffer\n");
        //     return 1;
        // }

        // auto main1 = GetMain(buffer);
        // std::cout << main1->testVector2()->Get(0)->str() << std::endl;

        // auto main2 = Main::Pack(nbuilder, &n);
        // nbuilder.Finish(main2);
        // send(sock, nbuilder.GetBufferPointer(), nbuilder.GetSize(), 0);
    }

    delete[] buffer;
}