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
    This is experiment transfering flatbuffer through RaMP
        - data is built using flatbuffers' object API specifically for RaMP
        - prepare, pollforaccept and close time are not included in the result of experiment now

    Setting:
        PAGING = 0;
        FAULT TOLERACE = 0;
*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./ramp_api_experiment path_to_config id container_size(in bytes)" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    size_t size = (size_t)atoi(argv[3]);
    
    //manager    
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);

    int64_t key = 0;
    int num_entries = (size*0.60)/(88);

    RampBuilder<struct MainT> *mb = new RampBuilder<struct MainT>(memory_manager);  // builder for root type
    struct MainT *m;  // target object 
    struct MainT *n;  // target object
    if (id == 0) {
        // setup object #1
        m = mb->CreateRoot(size);

        std::string value = "";
        while(value.size() != 32) {
            value.append("a");
        }
        const char *str = value.c_str();
        // build the data
        //RampAlloc *alloc = (RampAlloc *)((uint8_t *)m->start_);
        while (key < num_entries) {
            m->testVector2.push_back(m->CreaterString(str));
            //printf("Unused_past now is %p\n", alloc->unused_past);
            key++;
        }

        //printf("wait for another machine to be ready...\n");
        sleep(5);
        // start experiment
        auto start = std::chrono::high_resolution_clock::now();

        m->Prepare(1);
        while(!m->PollForAccept()) {}

        m->Transfer();
        while ((n = mb->PollForRoot()) == nullptr) {}  // #2
        std::cout << n->testVector2[num_entries-1] << std::endl;
        auto end = std::chrono::high_resolution_clock::now();
        n->Close();
        while(!m->PollForClose()) {};
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
    } else {
        // setup object #2
        n = mb->CreateRoot(size);

        std::string value = "";
        while(value.size() != 32) {
            value.append("b");
        }
        const char *str = value.c_str();
        // build the data
        while (key < num_entries) {
            n->testVector2.push_back(n->CreaterString(str));
            key++;
        }

        //printf("ready to start the experiment...\n");

        while ((m = mb->PollForRoot()) == nullptr) {}  // #1
        std::cout << m->testVector2[num_entries-1] << std::endl;
        n->Prepare(0);
        while(!n->PollForAccept()) {}
        n->Transfer();

        m->Close();  // #1
        while(!n->PollForClose()) {};
    }

    delete mb;
    delete memory_manager;
}