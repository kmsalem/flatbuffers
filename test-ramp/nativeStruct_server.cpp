#include <iostream>
#include <utility>
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "TestObj_api.h"
#include "flatbuffers/ramp_builder.h"
#include "flatbuffers/SAllocator.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//typedef std::basic_string<char, std::char_traits<char>, RampAllocator> String;

struct simpliest {
    int foo_;
    int bar_;
};

/// test structure
struct test_simple_struct : public flatbuffers::NativeTable
{
    using NativeTable::NativeTable;
    test_simple_struct():foo(0), bar(0) {}
    int foo;
    int bar;
    struct simpliest * sp;
    rString id;
    rString testString;
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
    
    //RampAllocator *alloc = (RampAllocator *)((uint8_t *)mt-sizeof(RampAllocator));
    RampAllocator *alloc = (RampAllocator *)((uint8_t *)mt->start_);

    printf("Address of the object is %p \n", mt);
    printf("1.unused_past stored in allocator is %p \n", alloc->unused_past);

    std::cout << "Address of foo is " << &mt->foo << std::endl;
    std::cout << sizeof(test_simple_struct) << std::endl;
    std::cout << sizeof(RampAllocator) << std::endl;

    mt->sp = CreateWith<simpliest>(mt);
    mt->sp->foo_ = 100;
    printf("2.unused_past stored in allocator is %p \n", alloc->unused_past);

    // Create the string
    SAllocator<char> sa = SAllocator<char>(alloc);
    rString my = rString("hello", sa);
    mt->id = my;

    std::cout << "My id is " << mt->id << std::endl;
    printf("Address of my String is %p \n", mt->id);
    printf("3.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->id = "hi thereaaaaaaa";  // this works fine
    //mt->id = "hi there there there";  // will cause segmeatation fault in receiving side when read
                                        // overwrite '=' operation to make it resisable???
    std::cout << "My id is " << mt->id << std::endl;
    printf("4.unused_past stored in allocator is %p \n", alloc->unused_past);

    //mt->testString = "hey this is my test string";
    mt->testString = rString("hey this is my test string", sa);
    std::cout << "My testString is: " << mt->testString << std::endl;
    printf("5.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->Prepare(1);
    while(!mt->PollForAccept()) {}
    mt->Transfer();
    while(!mt->PollForClose()) {};

    delete mtb;
    delete memory_manager;
}