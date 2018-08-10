#ifndef _SALLOCATOR_H_
#define _SALLOCATOR_H_

#include <string>
#include <unordered_map>
/*
    RampAlloc is actually a memory pool, 
      which stores relevant information about available memory
*/ 
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
    // For now, we'll use the next free address.
    void* result = unused_past;
    // But before we return, update the next free address.
    unused_past = (void*) ((char*)unused_past + bytes);
    LogAssert((uintptr_t)unused_past < (uintptr_t)((char*)addr + size), "OUT OF MEMORY");  // Error checking
    LogInfo("allocate(addr = %p, bytes = %p)", result, (void*) bytes);
    return result;
  }

  // deallocate actually does nothing for now
  void deallocate(void* addr, size_t bytes) noexcept {
    LogInfo("deallocate(addr = %p, bytes = %p)", addr, (void*) bytes);
  }

  void* pool_addr;
  size_t pool_size;
  void* unused_past; // make it public temporarily for debug usage 
private:
  /*
    * MemoryPool metadata and state.
    * The reason this may be different from the actual addr and size of the 
    * underlying memory is because we may choose to embed information in the memory address themselves
  */
//   void* pool_addr;
//   size_t pool_size;

  /**
    * The high watermark for allocated memory, this class implements a
    * simple allocation algorithm by simply pushing the boundary forward, on an
    * RDMA pull, we would only need to pull as much as a unused past
  */
  // void* unused_past;

  //the actualy size of the rdma memory region
  void* addr;
  size_t size;

};

template <class T>
class SAllocator {
public:
    int debug_level = 0;

    using value_type = T;
    using pointer = value_type*;
    using const_pointer = typename \
        std::pointer_traits<pointer>::template rebind<value_type const>;
    using void_pointer = typename \
        std::pointer_traits<pointer>::template rebind<void>;
    using const_void_pointer = typename \
        std::pointer_traits<pointer>::template rebind<const void>;
    using reference = value_type&;
    using const_reference = const reference;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap            = std::false_type;
    using is_always_equal                        = std::is_empty<SAllocator>;

    template <class U> friend class SAllocator;
    template <class U> struct rebind {typedef SAllocator<U> other;};

private:
    RampAlloc *pool_;
public:
    // Constructor.
    SAllocator(RampAlloc *pool) noexcept : pool_(pool) {}

    SAllocator() noexcept : debug_level(3), pool_(NULL) {}

    template <class U>
    SAllocator(SAllocator<U> const& a) noexcept :
        pool_(a.pool_) {}

    pointer allocate(size_type num) {
        if (pool_ == NULL) {
            return this->temp_allocate(num); 
        }
        return static_cast<pointer>(
            pool_->allocate(num * sizeof(value_type)));
    }
    void deallocate(pointer addr, size_type num) noexcept {
        if (pool_ == NULL) { return this->temp_deallocate(addr, num); }
        return pool_->deallocate(addr, num * sizeof(value_type));
    }

    pointer temp_allocate(size_type num) {
        size_t bytes = num * sizeof(value_type);
        if (debug_level == 3) {
            std::cerr << "Allocation is called on default SAllocator()" << std::endl;
        }
        if (debug_level < 2) {
            std::cerr << "WARNING: calling malloc(size = " << bytes
                      << ") via default SAllocator()." << std::endl;
        }
        return static_cast<pointer>(malloc(bytes));
    }
    void temp_deallocate(pointer addr, size_type num) noexcept {
        if (debug_level == 3) {
            std::cerr << "Deallocation is called on default SAllocator()" << std::endl;
        }
        if (debug_level < 2) {
            std::cerr << "WARNING: calling free(ptr = " << static_cast<void*>(addr)
                      << ") via default SAllocator()." << std::endl;
        }
        return free(static_cast<void*>(addr));
    }

    template <class U, class ...Args>
    void construct(U* p, Args&& ...args) {
        // This is placement new syntax.
        ::new(p) U(std::forward<Args>(args)...);
    }

    template <class U>
    void destroy(U* p) noexcept {
        p->~U();
    }

    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max();
    }

    SAllocator select_on_container_copy_construction() const {
        return *this;
    }
};

template <class T, class U>
bool operator==(
    SAllocator<T> const& alloc_a, SAllocator<U> const& alloc_b
) noexcept {
    // False for safety.
    // Later we should consider returning alloc_a.pool == alloc_b.pool.
    return false;
}

template <class T, class U>
bool operator!=(SAllocator<T> const& x, SAllocator<U> const& y) noexcept {
    return !(x == y);
}

// Will check later if we can avoid doing this
typedef basic_string<char, std::char_traits<char>, SAllocator<char>> rString;
template <class key, class value>
using rMap = std::unordered_map<key, value, hash<key>, equal_to<key>, SAllocator<std::pair<const key, value> > >;

namespace std {
    template<>
    struct hash<rString> {
        size_t operator () (const rString &str) const {
            return hash<string>{}(std::string(str.data()));
        }
    };
}

#endif // __SALLOCATOR_H__