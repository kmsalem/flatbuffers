#include <iostream>
#include <utility>
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "TestObj_generated.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5050
#define RDMA_enabled 1
using namespace my::test;
  
int main(int argc, char const *argv[])
{
#if RDMA_enabled

    if (argc < 3) {
        LogError("./test_RDMAMemory ../config.txt server_id size");
        return 1;
    }
    int server_id = atoi(argv[2]);
    int mem_size = atoi(argv[3]);
    RDMAMemoryManager* manager = new RDMAMemoryManager(argv[1], server_id);
    flatbuffers::FlatBufferBuilder builder(manager, mem_size);

    char *memory; // start address of RDMA segment
    while ((memory = builder.PollForRoot()) == nullptr) {}

    flatbuffers::Verifier verifier((uint8_t *)memory, sizeof(char)*1024);
    printf("Start verifying flatbuffer\n");
    bool verified = VerifyClassBuffer(verifier);  // this function is included in TestObf_generated

    if (!verified) {
        printf("Error verifying flatbuffer\n");
        //return;
    } else {
        printf("Verified successfully\n");
    }

    auto class1 = GetClass(memory);
    printf("Get class succesfully\n");

    //manager->close(rdma_memory->vaddr, rdma_memory->size, rdma_memory->pair);


#else
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "10.70.0.8", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024); // get the message from server

    flatbuffers::Verifier verifier((uint8_t *)buffer, sizeof(char)*1024);
    bool verified = VerifyClassBuffer(verifier);  // this function is included in TestObf_generated

    if (!verified) {
        printf("Error verifying flatbuffer\n");
        //return;
    }

    auto class1 = GetClass(buffer);
#endif

  /*
    Deserialize received flatbuffer
      Note that verify process is optional 
  */

    std::cout << class1->name() << std::endl;
    std::cout << class1->name()->str() << std::endl;
    std::cout << class1->students()->Get(1)->name() << std::endl;
    std::cout << class1->students()->Get(1)->name()->str() << std::endl;
    
#if RDMA_enabled
    printf("%s\n", memory);
    //manager->close(rdma_memory->vaddr, rdma_memory->size, rdma_memory->pair);
    builder.Close();

#else
    printf("%s\n", buffer);
#endif

    return 0;
}

