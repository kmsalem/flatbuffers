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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./pack_int num_entries" << std::endl;
        return 1;
    }

    int num_entries = atoi(argv[1]);
    int64_t key = 0;

    flatbuffers::FlatBufferBuilder builder;

    struct MainT *m = new MainT();;
    struct MainT *n;

    std::string value = "aaaaaaaaaa";
    while (key < num_entries) {
        m->testVector2.push_back(value);
        key++;
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto main = Main::Pack(builder, m);
    builder.Finish(main);

    n = GetMain(builder.GetBufferPointer())->UnPack();
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << "stored value is " << n->testVector2[num_entries-1] << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
    delete m;
}