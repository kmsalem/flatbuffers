#include <iostream>
#include <utility>
#include "simple_map_generated.h"
#include "flatbuffers/ramp_builder.h"
#include "flatbuffers/SAllocator_bumb.h"

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

using namespace Map::Experiment;

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

/*
    Check size usage of map.
    Note that this experiment is specifically for bumb allocator.  
*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./ramp_api_buddy_alloc_map_experiment path_to_config id num_entries container_size(in bytes) required_entries(optional)" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    int num_entries = atoi(argv[3]);
    // size_t size = (size_t)atoi(argv[4]);
    size_t cb2size = (size_t)atoi(argv[4]);
    int64_t length = (int64_t)atoi(argv[5]);
    
    //manager    
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);
    #if PAGING
    manager = memory_manager;
    initialize();

    int required_entries = atoi(argv[6]);
    //uniform number generator
    const int range_from  = 0;
    const int range_to    = num_entries - 1;
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(range_from, range_to);
    #endif

    int32_t key = 0;

    RampBuilder<struct MainT> *mb = new RampBuilder<struct MainT>(memory_manager);  // builder for root type
    struct MainT *m;  // target object 
    struct MainT *n;  // target object
    srand(time(0));
    if (id == 0) {
        // setup object #1
        m = mb->CreateRoot(cb2size);
            
        // build the data
        while (key < num_entries) {
            std::string value;
            for(int i = 0; i < length; ++i)
            {
                value += genRandom();
            }
            const char *str = value.c_str();
            m->testMap2[key] = m->CreaterString(str);
            key++;
        }
        std::cout << m->testMap2[num_entries-1] << std::endl;
        RampAlloc *alloc = (RampAlloc *)((uint8_t *)m->start_);
        // printf("Unused_past stored in allocator is %p \n", alloc->unused_past);
        std::cout << "Used size is " << (char const *)alloc->unused_past - (char const *)m->start_ << std::endl;

        // printf("wait for another machine to be ready...\n");
        usleep(5000000);
        // start experiment
        auto start = std::chrono::high_resolution_clock::now();

        m->Prepare(1);
    
        while(!m->PollForAccept()) {}
        
        m->Transfer();
        while ((n = mb->PollForRoot()) == nullptr) {}  // #2
        #if PAGING
        // any difference of using while or for loop?
        rString val;
        for (int i = 0; i < required_entries; ++i) {
            val = n->testVector2[i];
            // val = n->testVector2[distr(generator)];
            // std::cout << val << std::endl;
        }
        #endif
        // std::cout << n->testVector2[num_entries-1] << std::endl;
        n->Close();
        auto end = std::chrono::high_resolution_clock::now();
        // n->Close();
        //while(!m->PollForClose()) {};
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;
    } else {
        // setup object #2
        n = mb->CreateRoot(cb2size);
            
        // build the data
        while (key < num_entries) {
            std::string value;
            for(int i = 0; i < length; ++i)
            {
                value += genRandom();
            }
            const char *str = value.c_str();
            n->testMap2[key] = n->CreaterString(str);
            key++;
        }

        // n->Prepare(0);
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
        m->Close();  // #1
        std::cout << m->testMap2[num_entries-1] << std::endl;
        
        n->Prepare(0);
        while(!n->PollForAccept()) {}
        n->Transfer();

        // m->Close();  // #1
        //while(!n->PollForClose()) {};
    }

    delete mb;
    delete memory_manager;
    return 0;
}