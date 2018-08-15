#include "LRUCache_generated.h"

using namespace LRUCache;

class LRUmap {
 public:
  LRUmap(): Cache(nullptr) {}

  // initialize cache map with a target root object and capacity
  void initialize(cacheT *cache, int capacity) {
    if (capacity <= 0) {
      // invalid capacity
      return;
    }

    Cache = cache;
    Cache->head = nullptr;
    Cache->tail = nullptr;
    Cache->capacity = capacity;
  }

  void remote_initialize(cacheT *cache) {
    Cache = cache;
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
    if (Cache->head == nullptr) {
      assert(Cache->tail == nullptr);
      Cache->head = node;
      Cache->tail = node;
      node->prev = nullptr;
      node->next = nullptr;
    }
    node->next = Cache->head->next;
    node->prev = nullptr;
    Cache->head->prev = node;
    Cache->head = node;
  }

  void removeTail() {
    NodeT *last = Cache->tail;
    Cache->tail = last->prev;
    detachNode(last);
    Cache->cmap.erase(last->key);
    // should we delete the actual Node as well here?
    // delete(last);  cause error---> free() invalid pointer?
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

 private:
    cacheT * Cache;
};