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

int main(int argc, char const *argv[]) {
    int server_id = atoi(argv[2]);
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], server_id);
    RampBuilder<struct ClassT> *cb = new RampBuilder<struct ClassT>(memory_manager);
    struct ClassT * c;
    while ((c = cb->PollForRoot()) == nullptr) {}

    std::cout << "Class name is " << c->name << std::endl;
    std::cout << "Class teacher name is " << c->teacher->name << std::endl;
    std::cout << "Class teacher age is " << c->teacher->age << std::endl;
    std::cout << "Class has students: " << std::endl;
    std::cout << c->students[0]->name << std::endl;
    std::cout << c->students[1]->name << std::endl;

    c->Close();

    delete memory_manager;
}