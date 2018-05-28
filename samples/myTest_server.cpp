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
#define PORT 5050
#define RDMA_enabled 1
using namespace my::test;

int mode;

int main(int argc, char* argv[]) {

#if RDMA_enabled
    if (argc < 3) {
        LogError("./test_RDMAMemory ../config.txt server_id size");
        return 1;
    }
    int server_id = atoi(argv[2]);
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    int mem_size = atoi(argv[3]);

    //flatbuffers::RampAllocator ramp_alloc(memory_manager);
    flatbuffers::RampAllocator ramp_alloc(memory_manager);
    flatbuffers::FlatBufferBuilder builder(mem_size, &ramp_alloc);
    size_t size = mem_size;
    if (size < builder.GetSize()) {
        printf("Memory declared is not enough\n");
        return 0;        
    }

#else
    flatbuffers::FlatBufferBuilder builder;
#endif

  /*
    Serialize data into FlatBuffer
  */
  auto student_one = builder.CreateString("Yilia");
  auto student_two = builder.CreateString("Amber");
  auto student_three = builder.CreateString("Bob");

  auto yilia = CreatePerson(builder, student_one, Gender_Female, 18);
  printf("pointer after Yilia is %p \n", builder.GetCurrentBufferPointer());
  auto amber = CreatePerson(builder, student_two, Gender_Female, 19);
  printf("pointer after Amber is %p \n", builder.GetCurrentBufferPointer());
  auto bob = CreatePerson(builder, student_three, Gender_Male, 16);
  printf("pointer after Bob is %p \n", builder.GetCurrentBufferPointer());

  std::vector<flatbuffers::Offset<Person>> students_vector;
  students_vector.push_back(yilia);
  students_vector.push_back(amber);
  students_vector.push_back(bob);

//   auto student_four = builder.CreateString("Mary");
//   auto mary = CreatePerson(builder, student_four, Gender_Female, 17);
//   students_vector.push_back(mary);

  auto students = builder.CreateVector(students_vector);
  
  auto name = builder.CreateString("cs350");

  auto teacher_name = builder.CreateString("Lesley");
  auto teacher = CreatePerson(builder, teacher_name, Gender_Female, 30);
  auto myClass = CreateClass(builder, name, students, teacher);
  builder.Finish(myClass);  // done serialize the information

  
  // address will always change whenever you run this sample
  // however, the size is always the same if the data is not changed
  std::cout << builder.GetBufferPointer() << std::endl;
  auto class1 = GetClass(builder.GetBufferPointer());
  std::cout << class1->name() << std::endl;
  std::cout << class1->name()->str() << std::endl;

  std::cout << class1->students() << std::endl;
  std::cout << class1->students()->Get(0)->name() << std::endl;
  std::cout << class1->students()->Get(0)->name()->str() << std::endl;
  std::cout << class1->students()->Get(1)->name() << std::endl;
  std::cout << class1->students()->Get(1)->name()->str() << std::endl;
  std::cout << class1->students()->Get(2)->name() << std::endl;
  std::cout << class1->students()->Get(2)->name()->str() << std::endl;
  std::cout << class1->teacher() << std::endl;
  std::cout << class1->teacher()->name()->str() << std::endl;


#if RDMA_enabled

    // store the address of the buffer
    uint8_t * buff_pointer = builder.GetBufferPointer();
    uint8_t ** temp = (uint8_t **)ramp_alloc.addr;
    *temp = buff_pointer;


    // we need to transfer the whole segment
    printf("Preparing...\n");
    memory_manager->Prepare(ramp_alloc.addr, size, 1);
    RDMAMemory* rdma_memory = nullptr;
    printf("Prepare done\n");

    //int i = 0;
    while((rdma_memory = memory_manager->PollForAccept()) == nullptr) {
        //i++;
        //if(i >= (int)size)
        //    i = 0;    // what does this do?
    }
printf("Ready for transfer\n");
    memory_manager->Transfer(rdma_memory->vaddr, rdma_memory->size, rdma_memory->pair);
    memory_manager->PollForClose();

#else
  
    // Send FlatBuffer over network: using Socket to realize easy transfer 
  
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    //char *hello = "Hello from server";
      
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

    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    //send(new_socket , hello , strlen(hello) , 0 );
    send(new_socket, builder.GetBufferPointer(), builder.GetSize(), 0);
    printf("Hello message sent\n");
#endif

    return 0;
}
