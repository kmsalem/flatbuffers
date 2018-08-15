#include "LRUmap.h"
#include "flatbuffers/ramp_builder.h"

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
    RampBuilder<struct cacheT> *cb = new RampBuilder<struct cacheT>(memory_manager);
    LRUmap *Cache = new LRUmap();
    struct cacheT * c;
    if (id == 0) {
        c = cb->CreateRoot(12);  // I do not do this inside initialize because of RampBuilder

        Cache->initialize(c, 3);

        Cache->insert(1, "a");
        Cache->insert(2, "bbb");
        Cache->insert(3, "aba");
        std::cout << Cache->get(1) << std::endl;
        // rString n = Cache->CreaterString("nnnnnnnnnnnnnnnnnnnnn");
        // Cache->insert(6, n);
        Cache->insert(6, "mmmmmmmmmmmmmmmmmmmm");
        Cache->insert(5, "bdddd");
        Cache->insert(4, "dddd");
        
        // Cache->send(1);
        c->Prepare(1);
        while(!c->PollForAccept()) {}
        c->Transfer();
    } else {
        while ((c = cb->PollForRoot()) == nullptr) {}
        Cache->remote_initialize(c);

        std::cout << Cache->get(6) << std::endl;
        std::cout << Cache->get(4) << std::endl;
        std::cout << Cache->get(1) << std::endl;
        // std::cout << Cache->get(3) << std::endl;

        c->Close();
    }
    // delete Cache;
    delete memory_manager;
    delete cb;
}