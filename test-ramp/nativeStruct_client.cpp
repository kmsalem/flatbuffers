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
#include <unordered_map>

struct simple_vector : public flatbuffers::NativeTable {
    std::vector<int, SAllocator<int> > data;
    simple_vector(RampAlloc * alloc): data(SAllocator<int>(alloc)) {}
};

struct simpliest : public flatbuffers::NativeTable 
{
    // using NativeTable::NativeTable;
    int foo_;
    int bar_;
};

/// test structure
struct test_simple_struct : public flatbuffers::NativeTable
{
    // using NativeTable::NativeTable;
    int foo;
    int bar;
    struct simpliest * sp;
    rString id;
    rString testString;
    std::vector<int, SAllocator<int> > testVector;
    std::vector<struct simpliest *, SAllocator<struct simpliest *> > testVectorOfPointer;
    std::vector<rString, SAllocator<rString> > testVectorOfrString;
    std::vector<std::string, SAllocator<std::string> > testVectorOfString;
    std::vector<struct simple_vector *, SAllocator<struct simple_vector *> > testVectorOfVector;

    std::unordered_map<int, int, hash<int>, equal_to<int>, SAllocator<pair<const int, int> > > testMapInt;
    //std::unordered_map<rString, int, std::hash<int>, std::equal_to<rString>, SAllocator<std::pair<const rString, int> > > testMaprString;
    std::unordered_map<int, rString, hash<int>, equal_to<int>, SAllocator<pair<const int, rString> > > testMapIntrStr;

    //std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, SAllocator<std::pair<const std::string, int> > >testMapString;
    std::unordered_map<rString, int, hash<rString>, equal_to<rString>, SAllocator<pair<const rString, int> > > testMapString;
    //std::unordered_map<std::string, std::vector<int, SAllocator<int> >, std::hash<std::string>, std::equal_to<std::string>, SAllocator<std::pair<const std::string, std::vector<int, SAllocator<int> > > > > testMapVec;
    std::unordered_map<int, simple_vector *, hash<int>, equal_to<int>, SAllocator<pair<const int, simple_vector *> > > testMapObj;
    std::unordered_map<int, struct simpliest *, hash<int>, equal_to<int>, SAllocator<pair<const int, struct simpliest *> > > testMapTemp;
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
    
    std::cout << "Value of testVectorOfrString is: " << mt->testVectorOfrString[0];
    std::cout << mt->testVectorOfrString[1] << mt->testVectorOfrString[2] << mt->testVectorOfrString[3] << std::endl;

    std::cout << "Value of testVectorOfString is: " << mt->testVectorOfString[0] << mt->testVectorOfString[1] << std::endl;

    std::cout << "Value of testVectorOfVector is: " << mt->testVectorOfVector[0]->data[0] << " " << mt->testVectorOfVector[0]->data[1] << std::endl;

    std::cout << "--------Pair in testMapInt--------" << std::endl;
    std::cout << mt->testMapInt.bucket_count() << std::endl;
    std::cout << "0 " << mt->testMapInt[0] << std::endl;
    std::cout << "10 " << mt->testMapInt[10] << std::endl;

    std::cout << "--------Pair in testMapIntrStr--------" << std::endl;
    std::cout << mt->testMapIntrStr.bucket_count() << std::endl;
    std::cout << "22 " << mt->testMapIntrStr[22] << std::endl;
    std::cout << "99 " << mt->testMapIntrStr[99] << std::endl;


    std::cout << "--------Pair in testMapString--------" << std::endl;
    std::cout << mt->testMapString.bucket_count() << std::endl;
    std::cout << "yilia " << mt->testMapString["yilia"] << std::endl;
    std::cout << "bob " << mt->testMapString["bob"] << std::endl;
    std::cout << "aaaaa " << mt->testMapString["aaaaa"] << std::endl;   // this value does not exist

    std::cout << "--------Value in testMapObj--------" << std::endl;
    std::cout << "vec1 has " << mt->testMapObj[1]->data[0] << " " << mt->testMapObj[1]->data[1] << std::endl;
    std::cout << "vec2 has " << mt->testMapObj[2]->data[0] << " " << mt->testMapObj[2]->data[1] << std::endl;

    std::cout << "--------Value in testMapTemp--------" << std::endl;
    printf("Adrress of TempObj is %p \n", &mt->testMapTemp);
    std::cout << mt->testMapTemp.size() << std::endl;   // this is arbitrarily large, why?
    std::cout << mt->testMapTemp.bucket_count() << std::endl;
    std::cout << mt->testMapTemp.bucket_size(0) << std::endl; 
    
    auto it = mt->testMapTemp.find(1); // this function does not work
    if (it == mt->testMapTemp.end()) {
        std::cout << "Key cannot be found in the map" << std::endl;
        return 1;
    } else {
        std::cout << "first pair is " << it->first << ", " << it->second->foo_ << " " << it->second->bar_ << endl;
    }

    mt->Close();

    delete memory_manager;
}