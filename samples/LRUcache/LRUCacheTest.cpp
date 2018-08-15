#include "LRUmap.h"

using namespace LRUCache;

/*
    Tested case:
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

        Cache->initialize(memory_manager, 3);

        Cache->insert(1, "a");
        Cache->insert(2, "bbb");
        Cache->insert(3, "aba");
        std::cout << Cache->get(1) << std::endl;
        // rString n = Cache->CreaterString("nnnnnnnnnnnnnnnnnnnnn");
        // Cache->insert(6, n);
        Cache->insert(6, "mmmmmmmmmmmmmmmmmmmm");
        Cache->insert(5, "bdddd");
        Cache->insert(4, "dddd");
        
        Cache->send(1);  // this may not be what we want -- modify as required
        
    } else {
        Cache->remote_initialize(memory_manager);

        // std::cout << Cache->get(5) << std::endl;
        std::cout << Cache->get(6) << std::endl; // 6 4 5
        std::cout << Cache->get(4) << std::endl; // 4 6 5
        Cache->insert(1, "a"); // 1 4 6
        std::cout << Cache->get(1) << std::endl; // 1 4 6
        std::cout << Cache->get(5) << std::endl;
        std::cout << Cache->get(6) << std::endl;
        std::cout << Cache->get(4) << std::endl;
        std::cout << Cache->getCapacity() << std::endl;

        Cache->done();
    }
    delete Cache;
    delete memory_manager;
}