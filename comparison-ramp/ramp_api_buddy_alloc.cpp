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
    Check size usage of vector in buddy allocator.  
*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./ramp_api_experiment path_to_config id num_entries container_size(in bytes) required_entries(optional)" << std::endl;
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

    int64_t key = 0;

    RampBuilder<struct MainT> *mb = new RampBuilder<struct MainT>(memory_manager);  // builder for root type
    struct MainT *m;  // target object 
    struct MainT *n;  // target object
    if (id == 0) {
        // setup object #1
        m = mb->CreateRoot(cb2size);

        std::string value = "";
        while(value.size() != length) {
            value.append("a");
        }
        const char *str = value.c_str();
            
        // build the data
        // m->testVector2.reserve(num_entries);
        while (key < num_entries) {
            // m->testVector2.push_back(str);
            // m->testVector2[key] = m->CreaterString(str);
            m->testVector2.push_back(m->CreaterString(str));
            // m->testVector1.push_back(100);
            // printf("%d ", key);
            // printf("Number of element stored before next reallocation: %d\n", key);
            key++;
        }

        RampAlloc *alloc = (RampAlloc *)((uint8_t *)m->start_);
        std::cout << "Used size is " << alloc->CbUsed() << std::endl;
        std::cout << "Segment size needed is " << alloc->GetSize() << std::endl;

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

        std::string value = "";
        while(value.size() != length) {
            value.append("b");
        }
        const char *str = value.c_str();
        // build the data
        while (key < num_entries) {
            n->testVector2.push_back(n->CreaterString(str));
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
        std::cout << m->testVector2[num_entries-1] << std::endl;
        // std::cout << m->testVector3[0]->name << std::endl;
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