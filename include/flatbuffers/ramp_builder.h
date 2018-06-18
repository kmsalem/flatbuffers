/*
    This file is created by Yuxiao Yu on June 6, 2018
*/

#ifndef RAMP_BUILDER_H_
#define RAMP_BUILDER_H_

#include "assert.h"
#include "distributed-allocator/RDMAMemory.hpp"
#include "c++-containers/pool_based_allocator.hpp"

#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

// this is just an easy allocator using the same machenism as mempool in RDMA-migration-system 
class RampAlloc {
 public:
  explicit RampAlloc(void *pool_addr, 
                     size_t pool_size, 
                     void * unused_past,
                     void *addr, 
                     size_t size):     // put some default values if needed
            pool_addr(pool_addr),
            pool_size(pool_size), 
            unused_past(unused_past),
            addr(addr), 
            size(size) {}
  
  void* allocate(size_t bytes) noexcept {
    
    // LogInfo("allocate(bytes = %p) called on %p", (void*) bytes, this);

    // For now, we'll use the next free address.
    void* result = unused_past;
    // But before we return, update the next free address.
    unused_past = (void*) ((char*)unused_past + bytes);
    LogAssert((uintptr_t)unused_past < (uintptr_t)((char*)addr + size), "OUT OF MEMORY");
    return result;
  }

  // deallocate actually does nothing for now
  void deallocate(void* addr, size_t bytes) noexcept {
    LogInfo("deallocate(addr = %p, bytes = %p)",addr, (void*) bytes);
  }

  /**
    * MemoryPool metadata and state.
    * The reason this may be different from the actual addr and size of the 
    * underlying memory is because we may choose to embed information in the memory address themselves
    * this will not be required if we are using zookeeper
  */
  void* pool_addr;
  size_t pool_size;

  /**
    * The high watermark for allocated memory, this class implements a
    * simple allocation algorithm by simply pushing the boundary forward, on an
    * RDMA pull, we would only need to pull as much as a unused past
  */
  void* unused_past;

  //the actualy size of the rdma memory region
  void* addr;
  size_t size;

};

template<class T>
class RampBuilder {
 public:
  using root_type = T;

  RampBuilder(RDMAMemoryManager *manager) : manager_(manager) {}

  /// Create an object inside RDMA segment and initialize the allocator
  root_type * CreateRoot(size_t size) {
    void * memory = manager_->allocate(size);
    assert(is_aligned(memory,8));

    // build the allocator
    void *pool_addr = (void*)((char *)memory+sizeof(root_type)+sizeof(RampAlloc));
    RampAlloc *allocator = new (memory) 
                                RampAlloc(pool_addr,  
                                          size - (sizeof(root_type) + sizeof(RampAlloc)),
                                          pool_addr,
                                          memory, 
                                          size);

    // create a root object at the start of the segment
    root_type * root = new (memory+sizeof(RampAlloc)) T(manager_, (void *)memory, size);
    //root->manager_ = manager_;
    //root->start_ = (void *)memory;
    //root->size_ = size;  // need to see if this worth modification to compiler

    return root;
  }

  /// @brief Get the ownership of the buffer
  root_type * PollForRoot() {
    RDMAMemory * rdma_memory = manager_->PollForTransfer();
    if (rdma_memory == nullptr)
      return nullptr;
    
    root_type * root = (root_type *)(rdma_memory->vaddr+sizeof(RampAlloc));
    root->manager_ = manager_;
    root->rdma_memory = rdma_memory;
    root->start_ = rdma_memory->vaddr;
    return root;
    //return (root_type *)(rdma_memory->vaddr+sizeof(RampAlloc));
  }

 private:
  RDMAMemoryManager * manager_;
};

#endif // RAMP_BUILDER_H