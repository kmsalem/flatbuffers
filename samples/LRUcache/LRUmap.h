#include "LRUCache_generated.h"
#include "flatbuffers/ramp_builder.h"

using namespace LRUCache;

class LRUmap {
 public:
  LRUmap(): Cache(nullptr) {}

  // first initialize cache map with a target root object and capacity
  void initialize(RDMAMemoryManager* memory_manager, 
                  int capacity, 
                  uint8_t size = 12) {

    if (capacity <= 0) {
      // invalid capacity
      return;
    }

    RampBuilder<struct cacheT> *cb = new RampBuilder<struct cacheT>(memory_manager);
    Cache = cb->CreateRoot(size);

    Cache->head = nullptr;
    Cache->tail = nullptr;
    Cache->capacity = capacity;

    delete cb;
  }

  // initialize the map with received data
  void remote_initialize(RDMAMemoryManager* memory_manager) {
    RampBuilder<struct cacheT> *cb = new RampBuilder<struct cacheT>(memory_manager);
    while ((Cache = cb->PollForRoot()) == nullptr) {}

    delete cb;
  }

 // added method for implementing LRU cache

  void detachNode(NodeT *node) {
    if (node->prev == nullptr && node->next == nullptr) { // detach the only node
      Cache->head = nullptr;
      Cache->tail = nullptr;
    } else if (node->prev == nullptr) {  // detach first node
      Cache->head = node->next;
      Cache->head->prev = nullptr;
    } else if (node->next == nullptr) {  // detach last node
      Cache->tail = node->prev;
      Cache->tail->next = nullptr;
    } else {
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }
  }

  void putFront(NodeT *node) {  // how should this work?
    if (Cache->head == nullptr) {  // empty linked list
      assert(Cache->tail == nullptr);
      assert(node->prev == nullptr);
      assert(node->next == nullptr);
      Cache->head = node;
      Cache->tail = node;
      return;
    }
    node->next = Cache->head;
    Cache->head->prev = node;
    node->prev = nullptr;
    Cache->head = node;
  }

  void removeTail() {
    NodeT *last = Cache->tail;
    Cache->tail = last->prev;
    detachNode(last);
    Cache->cmap.erase(last->key);
    // delete the actual Node data
    Cache->DeleteObj(last);
  }

  rString get(int32_t key) {
    if (Cache->cmap.find(key) == Cache->cmap.end()) {
      // error here. Should find an elegant way to handle this
      return "Error: cannot find required key";
    }

    NodeT * node = Cache->cmap[key];
    detachNode(node);
    putFront(node);
    return node->value;
  }

  void insert(int32_t key, rString value) {
    if (Cache->cmap.find(key) != Cache->cmap.end()) {
        NodeT *current = Cache->cmap[key];

        detachNode(current);
        // move front
        current->value = value;
        putFront(current);
    } else {
        // not enough space
        if (Cache->cmap.size() >= Cache->capacity)
            removeTail();
    
        // add new value
        NodeT * newNode = Cache->CreateObj<NodeT>();
        newNode->key = key;
        newNode->value = value;
        Cache->cmap[key] = newNode;
        putFront(newNode);
    }
  }

  void insert(int32_t key, const char * value) {
    insert(key, Cache->CreaterString(value));
  }

  void send(int id) {
    Cache->Prepare(1);
    while(!Cache->PollForAccept()) {}
    Cache->Transfer();
  }

  void done() {
    Cache->Close();
  }

  // utility function
  int getCapacity() {
    return Cache->capacity;
  }

 private:
    cacheT * Cache;
};