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
#include <vector>
//#include <scoped_allocator>

struct simpliest : public flatbuffers::NativeTable 
{
    using NativeTable::NativeTable;
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
    rString id;
    rString testString;
    std::vector<int, SAllocator<int> > testVector;
    std::vector<struct simpliest *, SAllocator<struct simpliest *> > testVectorOfPointer;
};
  
int main(int argc, char const *argv[])
{
    if (argc < 3) {
        LogError("./test_RDMAMemory ../config.txt server_id size");
        return 1;
    }
    int server_id = atoi(argv[2]);
    //int mem_size = atoi(argv[3]);

    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    RampBuilder<struct test_simple_struct> *mtb = new RampBuilder<struct test_simple_struct>(memory_manager);
    struct test_simple_struct *mt;
    while ((mt = mtb->PollForRoot()) == nullptr) {}

    std::cout << "Value of foo is " << mt->foo << std::endl;
    std::cout << "Value of bar is " << mt->bar << std::endl;
    std::cout << "Value of sp->foo_ is " << mt->sp->foo_ << std::endl;
    std::cout << "Value of sp->bar_ is " << mt->sp->bar_ << std::endl;
    std::cout << "Value of id is " << mt->id << std::endl;
    std::cout << "Value of testString is " << mt->testString << std::endl;
    std::cout << "Value of testVector[0] is " << mt->testVector.at(0) << std::endl;
    std::cout << "Value of testVector[1] is " << mt->testVector.at(1) << std::endl;
    std::cout << "Value of testVectorOfPointer[0] foo is " << mt->testVectorOfPointer[0]->foo_ << std::endl;
    std::cout << "Value of testVectorOfPointer[1] bar is " << mt->testVectorOfPointer[1]->bar_ << std::endl;
    mt->Close();

    delete memory_manager;
}