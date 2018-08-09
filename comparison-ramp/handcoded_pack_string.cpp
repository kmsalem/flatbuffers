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
    srand(time(0));
    std::string value;

    for(int i = 0; i < length; ++i)
    {
        value += genRandom();
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<flatbuffers::Offset<flatbuffers::String>> svector;
    while (key < num_entries) {
        // std::string value;
        // for(int i = 0; i < length; ++i)
        // {
        //     value += genRandom();
        // }
        auto s = builder.CreateString(value);
        // std::cout << m->testVector2[key] <<std::endl;
        svector.push_back(s);
        key++;
    }

    auto v2 = builder.CreateVector(svector);
    auto main = CreateMain(builder, 0, v2, 0);
    builder.Finish(main);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Size of buffer is " << builder.GetSize() << std::endl;
    auto checkMain = GetMain(builder.GetBufferPointer());
    // std::cout << checkMain->testVector2()->Get(0)->str() << std::endl;
    std::cout << checkMain->testVector2()->Get(num_entries - 1)->str() << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
}