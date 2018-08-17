#include "LRUmap.h"
#include <utility>

using namespace LRUCache;

/*
    Tested case (correctness):
    1. simple -- evicit least used item when full
    2. capacity is 1
    3. literal string -- short and long
    4. rString -- short and long
*/
int main(int argc, const char * argv []) {
    int id = atoi(argv[2]);
    RDMAMemoryManager* memory_manager = new RDMAMemoryManager(argv[1], id);
    LRUmap *Cache = new LRUmap();
    if (id == 0) {
        // c = cb->CreateRoot(12);  // I do not do this inside initialize because of RampBuilder

        Cache->initialize(memory_manager, 9900, 21);

        /*
            Test block for performance
        */
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 10000; i++) {
            Cache->insert(i, "aaaaaaaaaa");
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Time cost is " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << std::endl;

        /*
            Test block for correctness
        */
        // Cache->insert(1, "a");
        // Cache->insert(2, "bbb");
        // Cache->insert(3, "aba");
        // std::cout << Cache->get(1) << std::endl;
        // // rString n = Cache->CreaterString("nnnnnnnnnnnnnnnnnnnnn");
        // // Cache->insert(6, n);
        // Cache->insert(6, "mmmmmmmmmmmmmmmmmmmm");
        // Cache->insert(5, "bdddd");
        // Cache->insert(4, "dddd");

        Cache->send(1);  // this may not be what we want -- modify as required
        
    } else {
        Cache->remote_initialize(memory_manager);

        /*
            Check data on receiving side to make sure data is settled inside segment correctly
        */
        // std::cout << Cache->get(5) << std::endl;
        // std::cout << Cache->get(6) << std::endl; // 6 4 5
        // std::cout << Cache->get(4) << std::endl; // 4 6 5
        // Cache->insert(1, "a"); // 1 4 6
        // std::cout << Cache->get(1) << std::endl; // 1 4 6
        // std::cout << Cache->get(9999) << std::endl;
        // std::cout << Cache->get(9990) << std::endl;
        // std::cout << Cache->get(9970) << std::endl;
        // std::cout << Cache->get(9870) << std::endl;
        // std::cout << Cache->getCapacity() << std::endl;

        Cache->done();
    }
    delete Cache;
    delete memory_manager;
}