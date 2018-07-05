#include <iostream>
#include <utility>
#include "vector_experiment_generated.h"
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

using namespace Main::Experiment;

/*
    PAGING = 1;
    FAULT TOLERACE = 0;
*/
int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "please provide all arguments" << std::endl;
        std::cerr << "./vector_experiment1 path_to_config id container_size(in bytes) bucket_size_in_milli_seconds" << std::endl;
        return 1;
    }

    int id = atoi(argv[2]);
    size_t size = (size_t)atoi(argv[3]);
    int bucket_size = atoi(argv[4]);
    
    //manager    
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);
    
    #if PAGING
    manager = memory_manager;
    initialize();
    printf("**************** Paging is on *****************\n");
    #endif

    int64_t key = 0;
    //number of keys
    /* Error would happen if we divide by size of integer (no experiment result)
        Page on: in sigsegv_advance : memory not found
        Page off: segmentation fault 
       If we divide by 8: OUT of MEMORY error on sending side but can still get experiment result*/
    int num_entries = (size*0.60)/(12);

    //uniform number generator
    const int range_from  = 0;
    const int range_to    = num_entries - 1;
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<int>  distr(range_from, range_to);

    RampBuilder<struct MainT> *mb = new RampBuilder<struct MainT>(memory_manager);  // builder for root type
    struct MainT *m;  // target object 
    if (id == 0) {
        m = mb->CreateRoot(size);
        
        // build the data
        while (key < num_entries) {
            m->testVector1.push_back((int32_t)key);
            key++;
        }

        // warmup?
        volatile int access = 0;
        int32_t val;
        while (access < 4000000) {
            val = m->testVector1[distr(generator)];
            access++;
        }

        m->Prepare(1);
        while(!m->PollForAccept()) {}
        m->Transfer();       // need to make sure PAGING is working
        while(!m->PollForClose()) {};
    } else {
        // setup?
        std::vector<double> times;        
        std::vector<std::vector<double>> bins;
    
        std::vector<int> bins_for_calls;

        while ((m = mb->PollForRoot()) == nullptr) {}

        int32_t val;
        double offset = 0.0;
        #if !PAGING
            // auto it = bins.begin();
            // std::vector<double> stop_and_copy; 
            // stop_and_copy.push_back(timer.get_duration_nsec());
            
            // std::cout << stop_and_copy << std::endl;
            // bins.push_back(stop_and_copy);
            offset = timer.get_duration_usec();
        #endif

        int bin = 0;
        // read data and measure time
        bins.push_back(*(new std::vector<double>()));
        bins_for_calls.push_back(0);

        double time_interval = bucket_size * 1000; // in micro seconds
        double time_passed = bucket_size * 1000; //starting from the first one
        auto start = std::chrono::high_resolution_clock::now();

        while(true) {
            auto end = std::chrono::high_resolution_clock::now();
            
            // you can set the time you want it to stop here 
            if (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() > (3000 - offset/(1000))) {
                break;
            } else if (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() > time_passed) {
                //bin it
                bins.push_back(*(new std::vector<double>()));
                bins_for_calls.push_back(0);
                bin++;
                time_passed += time_interval; 
                double x = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() + offset;
                times.push_back(x);
            }

            auto s1 = std::chrono::high_resolution_clock::now();
            val = m->testVector1[distr(generator)];   // where read data happens?
            auto e1 = std::chrono::high_resolution_clock::now();
            //if (val > 2000 && bin == 1) printf("---------------------------- val read is %d ------------------------\n", val);
            bins[bin].push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(e1 - s1).count());

            #if PAGING
            // if(remote_call){
            //     bins_for_calls[bin]++;
            //     remote_call = false;
            // } ???
            #endif            
        }

        m->Close();

        if (id != 0) {
            std::vector<double> mean_list;
            std::vector<double> nfivep_list;
            
            for (std::vector<double> bin : bins) {
                //sum of each bin
                double sum = 0.0;
                for (double x : bin)
                    sum += x;
                
                double mean = sum/((double)bin.size());
                mean_list.push_back(mean);
                std::sort(bin.begin(), bin.end());
                double nfivep = bin[((bin.size()*95)/100)];
                nfivep_list.push_back(nfivep);
            }
 
            // //final vals
            printf("time in milli second, mean_latency,95 percentile, throuhgput\n");
            for (unsigned int i=0; i< mean_list.size() - 2; i++) {
                printf(" %f, %f, %f, %d, %d\n", (double) (times[i]/1000), (double) (mean_list.at(i)/1000), (double) (nfivep_list.at(i)/1000), (int)bins[i].size(), bins_for_calls.at(i));
            }
        }
    }

    delete mb;
    delete memory_manager;
}