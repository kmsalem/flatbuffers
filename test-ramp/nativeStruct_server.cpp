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
#include <utility>

struct simple_vector : public flatbuffers::NativeTable {
    std::vector<int, SAllocator<int> > data;
    simple_vector(RampAlloc * alloc): data(SAllocator<int>(alloc)) {}
};

struct simpliest : public flatbuffers::NativeTable 
{
    //using NativeTable::NativeTable;
    int foo_;
    int bar_;
    simpliest(RampAlloc * alloc): foo_(0), bar_(0) {}
    simpliest() {}
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

    std::vector<int, SAllocator<int> > testVector;   // we still need the allocator for primitive type
    //std::vector<int> testVector;
    std::vector<struct simpliest *, SAllocator<struct simpliest *> > testVectorOfPointer;
    std::vector<rString, SAllocator<rString> > testVectorOfrString;
    std::vector<std::string, SAllocator<std::string> > testVectorOfString;  //this does work, why?
    std::vector<struct simple_vector *, SAllocator<struct simple_vector *> > testVectorOfVector;

    std::unordered_map<int, int, hash<int>, equal_to<int>, SAllocator<pair<const int, int> > > testMapInt;
    //std::unordered_map<rString, int, std::hash<rString>, std::equal_to<rString>, SAllocator<std::pair<const rString, int> > > testMaprString;
    std::unordered_map<int, rString, hash<int>, equal_to<int>, SAllocator<pair<const int, rString> > > testMapIntrStr;
    
    //std::unordered_map<std::string, int, std::hash<std::string>, std::equal_to<std::string>, SAllocator<std::pair<const std::string, int> > > testMapString;
    std::unordered_map<rString, int, hash<rString>, equal_to<rString>, SAllocator<pair<const rString, int> > > testMapString;
    //std::unordered_map<rString, int> testMapString;  // why this does not require custom hash and equal to
    //std::unordered_map<std::string, std::vector<int, SAllocator<int> >, std::hash<std::string>, std::equal_to<std::string>, SAllocator<std::pair<const std::string, std::vector<int, SAllocator<int> > > > > testMapVec;

    //std::unordered_map<std::string, simple_vector *, std::hash<std::string>, std::equal_to<std::string>, SAllocator<std::pair<const std::string, simple_vector *> > > testMapObj;
    std::unordered_map<int, struct simple_vector *, hash<int>, equal_to<int>, SAllocator<pair<const int, struct simple_vector *> > > testMapObj;
    std::unordered_map<int, struct simpliest *, hash<int>, equal_to<int>, SAllocator<pair<const int, struct simpliest *> > > testMapTemp;
    test_simple_struct(RampAlloc * alloc): foo(0), 
                                           bar(0), 
                                           id(SAllocator<rString>(alloc)),
                                           testString(SAllocator<rString>(alloc)),

                                           testVector(SAllocator<int>(alloc)),
                                           testVectorOfPointer(SAllocator<struct simpliest *>(alloc)),
                                           testVectorOfrString(SAllocator<rString>(alloc)),
                                           testVectorOfString(SAllocator<std::string>(alloc)),
                                           testVectorOfVector(SAllocator<struct simple_vector *>(alloc)),

                                           testMapInt(SAllocator<std::pair<const int, int> >(alloc)),
                                           testMapIntrStr(SAllocator<std::pair<const int, rString> >(alloc)),
                                           testMapString(SAllocator<std::pair<const rString, int> >(alloc)),
                                           //testMapVec(SAllocator<std::pair<const std::string, std::vector<int, SAllocator<int> > > >(alloc)),
                                           testMapObj(SAllocator<std::pair<const int, struct simple_vector *> >(alloc)),
                                           testMapTemp(SAllocator<std::pair<const int, struct simpliest *> >(alloc))
                                           {}
};

int main(int argc, char* argv[]) {

    if (argc < 3) {
        LogError("./test_RDMAMemory ../config.txt server_id size");
        return 1;
    }
    int server_id = atoi(argv[2]);

    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    RampBuilder<struct test_simple_struct> *mtb = new RampBuilder<struct test_simple_struct>(memory_manager);
    struct test_simple_struct *mt = mtb->CreateRoot(5120);
    mt->foo = 4;
    mt->bar = 2;
    
    RampAlloc *alloc = (RampAlloc *)((uint8_t *)mt->start_);

    printf("Address of the object is %p \n", mt);
    //printf("1.unused_past stored in allocator is %p \n", alloc->unused_past);

    std::cout << "Address of foo is " << &mt->foo << std::endl;
    std::cout << sizeof(test_simple_struct) << std::endl;
    std::cout << sizeof(RampAlloc) << std::endl;

    mt->sp = mt->CreateObj<simpliest>();
    mt->sp->foo_ = 101;
    //printf("2.unused_past stored in allocator is %p \n", alloc->unused_past);

    /* Create strings */

    //mt->id = mt->CreaterString("hello");
    mt->id = "hello";
    std::cout << "My id is " << mt->id << std::endl;
    printf("Address of my id is %p \n", mt->id);
    //printf("3.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->id = "hi thereaaaaaaa";  // this works fine
    //mt->id = "hi there there there there there there there";  // will not cause segmeatation fault; why?
    //mt->id = mt->CreaterString("hi there there there there there there there"); // Allocator will assign the same space as above line
    std::cout << "My id is " << mt->id << std::endl;
    //printf("4.unused_past stored in allocator is %p \n", alloc->unused_past);

    printf("Adrress of test string a is %p \n", &mt->testString);
    printf("Adrress of test string a is %p \n", &(*(mt->testString).begin()));
    //mt->testString = mt->CreaterString("hey this is my test string");
    mt->testString = "hey this is my test string";
    printf("Adrress of test string a is %p \n", &(*(mt->testString).begin()));
    std::cout << "My testString is: " << mt->testString << std::endl;
    //printf("5.unused_past stored in allocator is %p \n", alloc->unused_past);
    mt->testString = mt->testString + " this is appended string";


    /* Create vectors */
    //mt->testVector = mt->CreaterVector<int>();

    mt->testVector.push_back(11);
    std::cout << "Value of testVector[0] is " << mt->testVector.at(0) << std::endl;
    mt->testVector.push_back(13);
    printf("Address of testVector[0] is %p \n", &mt->testVector[0]);
    std::cout << "Value of testVector[1] is " << mt->testVector.at(1) << std::endl;
    //printf("6.unused_past stored in allocator is %p \n", alloc->unused_past);

    //mt->testVectorOfPointer = mt->CreaterVector<struct simpliest *>();
    struct simpliest * s1 = mt->CreateObj<simpliest>();
    struct simpliest * s2 = mt->CreateObj<simpliest>();
    mt->testVectorOfPointer.push_back(s1);
    mt->testVectorOfPointer.push_back(s2);
    mt->testVectorOfPointer[0]->foo_ = 1111;
    mt->testVectorOfPointer[1]->bar_ = 5;

    // rString r1 = mt->CreaterString("what ");
    // rString r2 = mt->CreaterString("a ");
    // rString r3 = mt->CreaterString("good ");
    // rString r4 = mt->CreaterString("day ");

    // mt->testVectorOfrString.push_back(r1);
    // mt->testVectorOfrString.push_back(r2);
    // mt->testVectorOfrString.push_back(r3);
    // mt->testVectorOfrString.push_back(r4);
    mt->testVectorOfrString.push_back("what ");
    mt->testVectorOfrString.push_back("a ");
    mt->testVectorOfrString.push_back("nice ");
    mt->testVectorOfrString.push_back("day ");
    printf("7.unused_past stored in allocator is %p \n", alloc->unused_past);

    mt->testVectorOfString.push_back("hello ");
    mt->testVectorOfString.push_back("world ");

    mt->testVectorOfVector.push_back(mt->CreateObj<simple_vector>());
    mt->testVectorOfVector[0]->data.push_back(111);
    mt->testVectorOfVector[0]->data.push_back(999);

    printf("Start building map...\n");
    mt->testMapInt[0] = 7;
    mt->testMapInt[10] = 8;
    
    mt->testMapIntrStr[22] = "yilia";
    mt->testMapIntrStr[99] = "bob";

    printf("8.unused_past stored in allocator is %p \n", alloc->unused_past);
    mt->testMapString["yilia"] = 18;
    mt->testMapString["bob"] = 17;
    printf("9.unused_past stored in allocator is %p \n", alloc->unused_past);

     /* This can only work if SAllocator propagate_on_container_move_assignment is true 
        Instead, use a map of tables containing a vector.
     */
    // mt->testMapVec["vec1"] = mt->CreaterVector<int>();
    // mt->testMapVec["vec2"] = mt->CreaterVector<int>();
    // mt->testMapVec["vec1"].push_back(0);
    // mt->testMapVec["vec1"].push_back(1);
    // mt->testMapVec["vec2"].push_back(100);
    // mt->testMapVec["vec2"].push_back(99);

    mt->testMapObj[1] = mt->CreateObj<simple_vector>();  // why this does not work?
    mt->testMapObj[1]->data.push_back(0);
    mt->testMapObj[1]->data.push_back(1);

    mt->testMapObj[2] = mt->CreateObj<simple_vector>();
    mt->testMapObj[2]->data.push_back(100);
    mt->testMapObj[2]->data.push_back(99);
    printf("Adrress of MapObj is %p \n", &mt->testMapObj);
    printf("Adrress of vector1 is %p \n", &mt->testMapObj[1]);
    printf("Adrress of vec1 data is %p \n", mt->testMapObj[1]->data.data());
    printf("Adrress of vec2 data is %p \n", mt->testMapObj[2]->data.data());

    std::cout << mt->testMapObj.bucket_count() << std::endl;
    std::cout << mt->testMapObj.size() << std::endl;
    std::cout << "vec1 has " << mt->testMapObj[1]->data[0] << " " << mt->testMapObj[1]->data[1] << std::endl;
    std::cout << "vec2 has " << mt->testMapObj[2]->data[0] << " " << mt->testMapObj[2]->data[1] << std::endl;

    mt->testMapTemp[1] = mt->CreateObj<simpliest>();
    mt->testMapTemp[1]->foo_ = 100;
    mt->testMapTemp[1]->bar_ = 999;

    mt->Prepare(1);
    while(!mt->PollForAccept()) {}
    mt->Transfer();
    while(!mt->PollForClose()) {};

    delete mtb;
    delete memory_manager;
}