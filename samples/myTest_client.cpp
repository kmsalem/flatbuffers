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
    RDMAMemoryManager* manager = new RDMAMemoryManager(argv[1], server_id);
    int memory_size = atoi(argv[3]);

    RDMAMemory* rdma_memory  = nullptr;
    while((rdma_memory = manager->PollForTransfer()) == nullptr) {}  // pulled size here is 256?

    // get the real address of the buffer
    uint8_t ** temp = (uint8_t **)rdma_memory->vaddr;
    char *memory = (char*)(*temp);

    if (rdma_memory == nullptr) {
        printf("error memory segment\n");
        return 0;
    }

    //char* memory = (char*)rdma_memory->vaddr;
    LogInfo("Pulled memory with %p with %s of size %zu", memory, memory, rdma_memory->size);  // size here is 1024

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
    
#if RDMA_enabled
    printf("%s\n", memory);
    manager->close(rdma_memory->vaddr, rdma_memory->size, rdma_memory->pair);
#else
    printf("%s\n", buffer);
#endif

    return 0;
}

