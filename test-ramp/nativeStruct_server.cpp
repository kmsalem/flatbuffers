#include <iostream>
#include <utility>
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "TestObj_api.h"
#include "flatbuffers/ramp_builder.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct simpliest {
    int foo_;
    int bar_;
};

/// test structure
struct test_simple_struct : public flatbuffers::NativeTable
{
    using NativeTable::NativeTable;
    int foo;
    int bar;
    struct simpliest * sp;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        LogError("./test_RDMAMemory ../config.txt server_id size");
        return 1;
    }
    int server_id = atoi(argv[2]);

    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    RampBuilder<struct test_simple_struct> *mtb = new RampBuilder<struct test_simple_struct>(memory_manager);
    struct test_simple_struct *mt = mtb->CreateRoot(1024);
    mt->foo = 4;
    mt->bar = 2;
    
    RampAllocator *alloc = (RampAllocator *)((uint8_t *)mt+sizeof(test_simple_struct));

    printf("Address of the object is %p \n", mt);
    printf("pool_addr stored in allocator is %p \n", alloc->pool_addr);
    printf("addr stored in allocator is %p \n", alloc->addr);

    std::cout << "Address of foo is " << &mt->foo << std::endl;
    std::cout << sizeof(test_simple_struct) << std::endl;
    std::cout << sizeof(RampAllocator) << std::endl;

    mt->sp = CreateWith<simpliest>(mt);
    mt->sp->foo_ = 100;

    printf("Address of sp is %p \n", mt->sp);
    printf("Address of sp->foo_ is %p \n", &mt->sp->foo_);

    mt->Prepare(1);
    while(!mt->PollForAccept()) {}
    mt->Transfer();
    while(!mt->PollForClose()) {};

    delete mtb;
    delete memory_manager;
}