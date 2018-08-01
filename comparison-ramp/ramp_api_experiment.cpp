#include <iostream>
#include <utility>
#include "ramp_api_generated.h"
#include "flatbuffers/ramp_builder.h"
#include "flatbuffers/SAllocator.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include <random>
#include <chrono>
#include <thread>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace Comparison::Experiment;

/*
    This is experiment transfering structured data through RaMP
        - data is built using flatbuffers' object API specifically for RaMP
        - prepare, pollforaccept and close time are not included in this version of experiment

    Setting:
        PAGING = 0 or 1;
        FAULT TOLERACE = 0;

    Required argument:
    0. ./ramp_api_expriment
    1. path to configuration file
    2. total number of elements stored in vector
    3. estimated size of RaMP segment
    4. number of entries you want to access for PAING on experiment (optional)
*/
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./ramp_api_experiment path_to_config id num_entries container_size(in bytes) required_entries(optional)" << std::endl;
        return 1;
    }

    // #if PREFETCHING
    // std::cout << "Prefectching is on..." << std::endl;
    // #endif

    // #if ASYNC_PREFETCHING
    // std::cout << "ASYNC_Prefectching is on..." << std::endl;
    // #endif

    int id = atoi(argv[2]);
    int num_entries = atoi(argv[3]);
    size_t size = (size_t)atoi(argv[4]);
    
    //manager    
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);
    #if PAGING
    manager = memory_manager;
    initialize();

    int required_entries = atoi(argv[5]);
    //uniform number generator
    const int range_from  = 0;
    const int range_to    = num_entries - 1;
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(range_from, range_to);
    #endif

    int64_t key = 0;

    RampBuilder<struct MainT> *mb = new RampBuilder<struct MainT>(memory_manager);  // builder for root type
    struct MainT *m;  // target object 
    struct MainT *n;  // target object
    if (id == 0) {
        // setup object #1
        m = mb->CreateRoot(size);

        std::string value = "";
        while(value.size() != 10) {
            value.append("a");
        }
        const char *str = value.c_str();

        // build the data
        while (key < num_entries) {
            m->testVector2.push_back(m->CreaterString(str));
            //printf("Unused_past now is %p\n", alloc->unused_past);
            key++;
        }

        // RampAlloc *alloc = (RampAlloc *)((uint8_t *)m->start_);
        // printf("Unused_past stored in allocator is %p \n", alloc->unused_past);
        // std::cout << "Used size is " << (char const *)alloc->unused_past - (char const *)m->start_ << std::endl;

        m->Prepare(1);
        while(!m->PollForAccept()) {}
        // printf("wait for another machine to be ready...\n");
        usleep(5000000);
        // start experiment
        auto start = std::chrono::high_resolution_clock::now();
        
        m->Transfer();
        while ((n = mb->PollForRoot()) == nullptr) {}  // #2
        // std::cout << n->testVector2[num_entries-1] << std::endl;
        #if PAGING
        // any difference of using while or for loop?
        rString val;
        for (int i = 0; i < required_entries; ++i) {
            val = n->testVector2[i];
            // val = n->testVector2[distr(generator)];
            // std::cout << val << std::endl;
        }
        #endif
        auto end = std::chrono::high_resolution_clock::now();
        n->Close();
        //while(!m->PollForClose()) {};
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    } else {
        // setup object #2
        n = mb->CreateRoot(size);

        std::string value = "";
        while(value.size() != 10) {
            value.append("b");
        }
        const char *str = value.c_str();
        // build the data
        while (key < num_entries) {
            n->testVector2.push_back(n->CreaterString(str));
            key++;
        }

        n->Prepare(0);
        while(!n->PollForAccept()) {}
        // printf("ready to start the experiment...\n");

        while ((m = mb->PollForRoot()) == nullptr) {}  // #1
        #if PAGING
        // any difference of using while or for loop?
        rString val;
        for (int i = 0; i < required_entries; ++i) {
            val = m->testVector2[i];
            // val = m->testVector2[distr(generator)];
            // std::cout << val << std::endl;
        }
        #endif
        // std::cout << m->testVector2[num_entries-1] << std::endl;
        n->Transfer();
        m->Close();  // #1
        //while(!n->PollForClose()) {};
    }

    delete mb;
    delete memory_manager;
    return 0;
}