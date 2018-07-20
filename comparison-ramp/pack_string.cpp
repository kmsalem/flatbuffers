#include <iostream>
#include <utility>
#include "obj_api_generated.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <random>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <time.h>

using namespace Comparison::Experiment;

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;

char genRandom()
{
    return alphanum[rand() % stringLength];
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./pack_string num_entries length_of_string" << std::endl;
        return 1;
    }

    int num_entries = atoi(argv[1]);
    int length = atoi(argv[2]);
    int64_t key = 0;

    flatbuffers::FlatBufferBuilder builder;

    struct MainT *m = new MainT();;
    struct MainT *n;

    // case 1
    // std::string value = "aaaaaaaaaa";
    // case 2: generate random string
    srand(time(0));
    std::string value;
    for(int i = 0; i < length; ++i)
    {
        value += genRandom();
    }

    while (key < num_entries) {
        // std::string value;
        // for(int i = 0; i < length; ++i)
        // {
        //     value += genRandom();
        // }
        m->testVector2.push_back(value);
        // std::cout << m->testVector2[key] <<std::endl;
        key++; 
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto main = Main::Pack(builder, m);
    builder.Finish(main);

    n = GetMain(builder.GetBufferPointer())->UnPack();
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "size of packed buffer is " << builder.GetSize() << std::endl;
    // std::cout << "stored value is " << n->testVector2[num_entries-1] << std::endl;
    std::cout << "stored value has length " << n->testVector2[num_entries-1].size() << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
    delete m;
}