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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./flatbuffer_ramp_experiment path_to_config id container_size(in bytes)" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    size_t size = (size_t)atoi(argv[3]);

    int64_t key = 0;
    int num_entries = (size*0.60)/(88);

    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);

    flatbuffers::FlatBufferBuilder mbuilder(memory_manager, size);
    flatbuffers::FlatBufferBuilder nbuilder(memory_manager, size);

    char *memory; // start address of RDMA segment
    
    struct MainT *m;
    struct MainT *n;
    if (id == 0) {
        // setup object#1
        m = new MainT();
        std::string str = "";
        while(str.size() != 32) {
            str.append("a");
        }
        // build the data
        while (key < num_entries) {
            m->testVector2.push_back(str);
            key++;
        }

        //printf("Wait for another machine to be ready...\n");
        sleep(5);
        auto start = std::chrono::high_resolution_clock::now();

        auto main1 = Main::Pack(mbuilder, m);
        mbuilder.Finish(main1);

        mbuilder.Prepare(1);
        while(!mbuilder.PollForAccept()) {}

        mbuilder.Transfer();
        while ((memory = nbuilder.PollForRoot()) == nullptr) {}
        n = GetMain(memory)->UnPack();
        auto end = std::chrono::high_resolution_clock::now();

        // std::cout << n->testVector2[0] << std::endl;
        // std::cout << n->testVector2[num_entries-1] << std::endl;

        nbuilder.Close();
        while(mbuilder.PollForClose()) {};  // we do not really need PollForClose

        //printf("time cost is %f \n", (double)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        delete m;
    } else {
        // setup object#1
        n = new MainT();
        std::string str = "";
        while(str.size() != 32) {
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
        // std::cout << m->testVector2[0] << std::endl;

        auto main2 = Main::Pack(nbuilder, n);
        nbuilder.Finish(main2);
        nbuilder.Prepare(0);
        while(!nbuilder.PollForAccept()) {}

        nbuilder.Transfer();

        mbuilder.Close();
        while(nbuilder.PollForClose()) {};
        delete n;
    }

    delete memory_manager;
}