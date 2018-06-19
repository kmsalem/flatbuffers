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

using namespace my::test;

// config.txt has information of send/receive machines

int main(int argc, char* argv[]) {
    int server_id = atoi(argv[2]);
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    RampBuilder<struct ClassT> *cb = new RampBuilder<struct ClassT>(memory_manager);

    struct ClassT *c = cb->CreateRoot(1024);
    c->name = c->CreaterString("CS350");
    
    c->teacher = c->CreateObj<struct PersonT>();
    c->teacher->name = c->teacher->CreaterString("Lesley");  // it does not matter which object create the object?
    c->teacher->age = 35;

    struct PersonT * s1 = c->CreateObj<struct PersonT>();
    struct PersonT * s2 = c->CreateObj<struct PersonT>();
    s1->name = s1->CreaterString("Yilia");
    s2->name = s2->CreaterString("Bob");

    c->students = c->CreaterVector<struct PersonT *>();
    c->students.push_back(s1);
    c->students.push_back(s2);

    c->Prepare(1);
    while(!c->PollForAccept()) {}
    c->Transfer();
    while(!c->PollForClose()) {};

    delete cb;
    delete memory_manager;
}