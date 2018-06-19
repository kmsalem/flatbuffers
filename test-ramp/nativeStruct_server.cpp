#include <iostream>
#include <utility>
#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "flatbuffers/ramp_builder.h"
#include "flatbuffers/SAllocator.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <scoped_allocator>
//typedef std::basic_string<char, std::char_traits<char>, RampAllocator> String;

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
    test_simple_struct():foo(0), bar(0) {}
    int foo;
    int bar;
    struct simpliest * sp;
    rString id;
    rString testString;
    std::vector<int, SAllocator<int> > testVector;
    std::vector<struct simpliest *, SAllocator<struct simpliest *> > testVectorOfPointer;
    std::vector<rString, SAllocator<rString> > testVectorOfString;
};

int main(int argc, char* argv[]) {

    if (argc < 3) {
        LogError("./test_RDMAMemory ../config.txt server_id size");
        return 1;
    }
    int server_id = atoi(argv[2]);

    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    RampBuilder<struct test_simple_struct> *mtb = new RampBuilder<struct test_simple_struct>(memory_manager);
    struct test_simple_struct *mt = mtb->CreateRoot(1024);  // default allocator will be called
    mt->foo = 4;
    mt->bar = 2;
    
    RampAlloc *alloc = (RampAlloc *)((uint8_t *)mt->start_);

    printf("Address of the object is %p \n", mt);
    printf("1.unused_past stored in allocator is %p \n", alloc->unused_past);

    std::cout << "Address of foo is " << &mt->foo << std::endl;
    std::cout << sizeof(test_simple_struct) << std::endl;
    std::cout << sizeof(RampAlloc) << std::endl;

    mt->sp = mt->CreateObj<simpliest>();
    mt->sp->foo_ = 101;
    printf("2.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->id = mt->CreaterString("hello");

    std::cout << "My id is " << mt->id << std::endl;
    printf("Address of my id is %p \n", mt->id);
    printf("3.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->id = "hi thereaaaaaaa";  // this works fine
    //mt->id = "hi there there there there there there there";  // will not cause segmeatation fault; why?
    //mt->id = mt->CreaterString("hi there there there there there there there"); // Allocator will assign the same space as above line
    std::cout << "My id is " << mt->id << std::endl;
    printf("4.unused_past stored in allocator is %p \n", alloc->unused_past);

    printf("Adrress of test string a is %p \n", &mt->testString);
    printf("Adrress of test string a is %p \n", &(*(mt->testString).begin()));
    mt->testString = mt->CreaterString("hey this is my test string");
    printf("Adrress of test string a is %p \n", &(*(mt->testString).begin()));
    std::cout << "My testString is: " << mt->testString << std::endl;
    printf("5.unused_past stored in allocator is %p \n", alloc->unused_past);
    mt->testString = mt->testString + " this is appended string";

    mt->testVector = mt->CreaterVector<int>();

    mt->testVector.push_back(11);
    std::cout << "Value of testVector[0] is " << mt->testVector.at(0) << std::endl;
    mt->testVector.push_back(13);
    printf("Address of testVector[0] is %p \n", &mt->testVector[0]);
    std::cout << "Value of testVector[1] is " << mt->testVector.at(1) << std::endl;
    printf("6.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->testVectorOfPointer = mt->CreaterVector<struct simpliest *>();
    struct simpliest * s1 = mt->CreateObj<simpliest>();
    struct simpliest * s2 = mt->CreateObj<simpliest>();
    mt->testVectorOfPointer.push_back(s1);
    mt->testVectorOfPointer.push_back(s2);
    mt->testVectorOfPointer[0]->foo_ = 1111;
    mt->testVectorOfPointer[1]->bar_ = 5;

    mt->testVectorOfString = mt->CreaterVector<rString>();
    rString r1 = mt->CreaterString("what ");
    rString r2 = mt->CreaterString("a ");
    rString r3 = mt->CreaterString("good ");
    rString r4 = mt->CreaterString("day ");

    mt->testVectorOfString.push_back(r1);
    mt->testVectorOfString.push_back(r2);
    mt->testVectorOfString.push_back(r3);
    mt->testVectorOfString.push_back(r4);

    mt->Prepare(1);
    while(!mt->PollForAccept()) {}
    mt->Transfer();
    while(!mt->PollForClose()) {};

    delete mtb;
    delete memory_manager;
}