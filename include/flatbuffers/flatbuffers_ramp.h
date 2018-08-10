/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FLATBUFFERS_H_
#define FLATBUFFERS_H_

#include "distributed-allocator/RDMAMemory.hpp"
#include "flatbuffers/SAllocator.h"
#include <vector>

namespace flatbuffers_ramp {

// Base class for native objects
struct NativeTable {
 public:
  explicit NativeTable() {}

  /* This function is not used any more */
  // This constructor is only for root type
  // any other user-defined structure inside root object should be created by CreateObj() method
  explicit NativeTable(RDMAMemoryManager *manager, 
                       void * start, 
                       size_t size): manager_(manager), 
                                     start_(start), 
                                     size_(size) {}

  /*
    Methods for creating data inside native structures
  */

  rString CreaterString(const char *str, size_t len) {
    RampAlloc *alloc = (RampAlloc *)((uint8_t *)start_);
    SAllocator<char> sa = SAllocator<char>(alloc);
    return rString(str, sa);
  }

  rString CreaterString(const char *str) {
    return CreaterString(str, strlen(str));
  }

  template <typename T>
  T * CreateObj() {
    RampAlloc *alloc = (RampAlloc *)((uint8_t *)start_);
    void * addr = alloc->allocate(sizeof(T));
    //T *result = new (addr) T(manager_, start_, size_);
    T *result = new (addr) T(alloc);
    result->manager_ = manager_;
    result->start_ = start_;
    result->size_ = size_;
    return result;
  }

  /* This function is not used any more */
  template <typename T>
  std::vector<T, SAllocator<T> > CreaterVector() {
    RampAlloc *alloc = (RampAlloc *)((uint8_t *)start_);
    SAllocator<T> va = SAllocator<T>(alloc);
    return std::vector<T, SAllocator<T> >(va);
  }

  /*
    Methods for transfer the buffer using RDMA-migration-system
  */

  /// @brief Prepare for sending the buffer
  void Prepare(int id) {
    manager_->Prepare((void *)start_, size_, id);
  }

  /// @brief Sending request to receiving server
  bool PollForAccept() {
    rdma_memory = manager_->PollForAccept();
    if (rdma_memory == nullptr)
      return false;

    return true;
  }

  /// @brief Transfer the ownership of buffer
  void Transfer() {
    if (rdma_memory == nullptr) {
      // error handling here
    }
    manager_->Transfer(rdma_memory->vaddr, rdma_memory->size, rdma_memory->pair);
  }

  /// @brief Close the connection
  bool PollForClose() {
    rdma_memory = manager_->PollForClose();
    if (rdma_memory == nullptr)
      return false;

    return true;
  }

  /*
    Methods for obtaining the buffer transferred through RDMA-migration-system
  */

  void Pull() {
    // this function does nothing now
  }

  /// @brief Close the connection
  void Close() {
    if (rdma_memory == nullptr) {
      // error handling here
    }
    manager_->close(rdma_memory->vaddr, rdma_memory->size, rdma_memory->pair);
  }
 
  RDMAMemoryManager * manager_;
  RDMAMemory * rdma_memory;

  void * start_; // start address of RAMP segment the object is in (where mempool info is stored)
  size_t size_; // size of RDMA segment
};

}  // namespace flatbuffers

#endif  // FLATBUFFERS_H_
