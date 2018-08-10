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
  root_type * CreateRoot(uint8_t cb2size, uint8_t cb2Min = 4) {
    size_t size = 1 << cb2size;
    void * memory = manager_->allocate(size);
    assert(is_aligned(memory,8));

    // Build the allocator
    RampAlloc *allocator = new (memory) RampAlloc(memory, 
                                                  cb2size,
                                                  cb2Min);

    // Create a root object at the start of the segment
    root_type * root = new (memory+sizeof(RampAlloc)+allocator->GetTsize()) T(allocator);
    root->manager_ = manager_;
    root->start_ = (void *)memory;
    root->size_ = size;  // Can we avoid assigning these fields repeatedly?

    // Set the bitmap of first few chunks that are used by allocator itself and root object
    void *result = allocator->allocate(sizeof(root_type) + sizeof(RampAlloc)+allocator->GetTsize());
    // std::cout << "Initial size needed is " << sizeof(root_type) + sizeof(RampAlloc) << std::endl;
    // printf("Address of memory is %p \n", memory);
    // printf("Address of assigned allocator is %p \n", result);
    assert(result == memory);

    return root;
  }

  /// @brief Get the ownership of the buffer
  root_type * PollForRoot() {
    RDMAMemory * rdma_memory = manager_->PollForTransfer();
    if (rdma_memory == nullptr)
      return nullptr;

    RampAlloc *allocator = (RampAlloc *)((uint8_t *)rdma_memory->vaddr);
    root_type * root = (root_type *)(rdma_memory->vaddr+sizeof(RampAlloc)+allocator->GetTsize());
    root->manager_ = manager_;
    root->rdma_memory = rdma_memory;
    root->start_ = rdma_memory->vaddr;
    return root;
  }

  /*
    Below are two methods for bumb allocator. 
    Instead of using above methods, using these two when you use the allocator in SAllocator_bumb.h.
  */
  // root_type * CreateRoot(size_t size) {
  //   void * memory = manager_->allocate(size);
  //   assert(is_aligned(memory,8));

  //   // Build the allocator
  //   void *pool_addr = (void*)((char *)memory+sizeof(root_type)+sizeof(RampAlloc));
  //   RampAlloc *allocator = new (memory) 
  //                               RampAlloc(pool_addr,  
  //                                         size - (sizeof(root_type) + sizeof(RampAlloc)),
  //                                         pool_addr,
  //                                         memory, 
  //                                         size);
  
  //   // Create a root object at the start of the segment
  //   root_type * root = new (memory+sizeof(RampAlloc)) T(allocator);
  //   root->manager_ = manager_;
  //   root->start_ = (void *)memory;
  //   root->size_ = size;  // Can we avoid assigning these fields repeatedly?

  //   return root;
  // }

  // root_type * PollForRoot() {
  //   RDMAMemory * rdma_memory = manager_->PollForTransfer();
  //   if (rdma_memory == nullptr)
  //     return nullptr;
    
  //   root_type * root = (root_type *)(rdma_memory->vaddr+sizeof(RampAlloc));
  //   root->manager_ = manager_;
  //   root->rdma_memory = rdma_memory;
  //   root->start_ = rdma_memory->vaddr;
  //   return root;
  // }

 private:
  RDMAMemoryManager * manager_;
};

#endif // RAMP_BUILDER_H