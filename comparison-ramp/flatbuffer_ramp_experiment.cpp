#include <iostream>
#include <utility>
#include "obj_api_generated.h"

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
    This is experiment transfering normal flatbuffer through RaMP
        - data is built locally first and then packed into flatbuffer before being sent
        - data is unpacked into native c++ structure after being received 

    Setting:
        PAGING = 0 or 1;
        FAULT TOLERACE = 0;

    In this version of the experiment, time cost of prepare and pollforaccept have to be included
        as one segment can only has one root object which can only be prepared to be sent after this object is built 
*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./flatbuffer_ramp_experiment path_to_config id num_entries container_size(in bytes) required_entries(optional)" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    int num_entries = atoi(argv[3]);
    size_t size = (size_t)atoi(argv[4]);

    int64_t key = 0;

    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);  // one manager can manage several RDMA memory segments
    
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

    flatbuffers::FlatBufferBuilder mbuilder(memory_manager, size);
    flatbuffers::FlatBufferBuilder nbuilder(memory_manager, size);

    char *memory; // start address of RDMA segment
    
    struct MainT *m;
    struct MainT *n;
    if (id == 0) {
        // setup object#1
        m = new MainT();
        std::string str = "";
        while(str.size() != 10) {
            str.append("a");
        }
        // build the data
        while (key < num_entries) {
            m->testVector2.push_back(str);
            key++;
        }

        //printf("Wait for another machine to be ready...\n");
        usleep(5000000);
        auto start = std::chrono::high_resolution_clock::now();

        auto main1 = Main::Pack(mbuilder, m);
        mbuilder.Finish(main1);
        
        mbuilder.Prepare(1);
        while(!mbuilder.PollForAccept()) {}

        mbuilder.Transfer();
        while ((memory = nbuilder.PollForRoot()) == nullptr) {}
        n = GetMain(memory)->UnPack();
        #if PAGING
        // any difference of using while or for loop?
        std::string val;
        for (int i = 0; i < required_entries; ++i) {
            val = n->testVector2[distr(generator)];
            // std::cout << val << std::endl;
        }
        #endif
        nbuilder.Close();
        auto end = std::chrono::high_resolution_clock::now();

        // std::cout << n->testVector2[0] << std::endl;
        // std::cout << n->testVector2[num_entries-1] << std::endl;

        // while(mbuilder.PollForClose()) {};  // we do not really need PollForClose

        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        delete m;
    } else {
        // setup object#1
        n = new MainT();
        std::string str = "";
        while(str.size() != 10) {
            str.append("b");
        }
        // build the data
        while (key < num_entries) {
            n->testVector2.push_back(str);
            key++;
        }

        //printf("Ready to start the experiment...\n");
        
        while ((memory = mbuilder.PollForRoot()) == nullptr) {}
        // unpack
        m = GetMain(memory)->UnPack();
        #if PAGING
        // any difference of using while or for loop?
        std::string val;
        for (int i = 0; i < required_entries; ++i) {
            val = m->testVector2[distr(generator)];
            // std::cout << val << std::endl;
        }
        #endif
        mbuilder.Close();

        // std::cout << n->testVector2[num_entries-5] << std::endl;
        
        auto main2 = Main::Pack(nbuilder, n);
        nbuilder.Finish(main2);
        // std::cout << GetMain(nbuilder.GetBufferPointer())->testVector2()->Get(num_entries-5)->str() << std::endl;

        nbuilder.Prepare(0);
        while(!nbuilder.PollForAccept()) {}

        nbuilder.Transfer();

        // while(nbuilder.PollForClose()) {};
        delete n;
    }

    delete memory_manager;   // this is the line causing segmentation fault(memory not found in memory map)?
    return 0;
}