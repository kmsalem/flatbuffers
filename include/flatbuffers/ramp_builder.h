/*
    This file is created by Yuxiao Yu on June 6, 2018
*/

#ifndef RAMP_BUILDER_H_
#define RAMP_BUILDER_H_

#include "assert.h"
#include "distributed-allocator/RDMAMemory.hpp"
#include "c++-containers/pool_based_allocator.hpp"
#include "flatbuffers/SAllocator.h"

#define is_aligned(POINTER, BYTE_COUNT) \
    (((uintptr_t)(const void *)(POINTER)) % (BYTE_COUNT) == 0)

/*
  RampBuilder is used to create a new "domain" (= RAMP segment), 
  and create a mempool as well as root object in this domain.
  Note: a builder can create one or more root objects (RAMP segments)
*/
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
    //root_type * root = new (memory+sizeof(RampAlloc)) T(manager_, (void *)memory, size);
    root_type * root = new (memory+sizeof(RampAlloc)) T(allocator);
    root->manager_ = manager_;
    root->start_ = (void *)memory;
    root->size_ = size;  // need to see if this worth modification to compiler

    /*
    SAllocator<root_type> ra = SAllocator<root_type>(allocator);
    using rebound_alloc_t =
            typename SAllocator<root_type>::template rebind<rString>::other;
    auto rebound = rebound_alloc_t(allocator);
    root_type * root = static_cast<root_type *>(rebound.allocate(sizeof(T)));
    rebound.construct(root, std::forward<Args>(args)...);
    */
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
  }

 private:
  RDMAMemoryManager * manager_;
};

#endif // RAMP_BUILDER_H