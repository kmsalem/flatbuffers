#ifndef _SALLOCATOR_H_
#define _SALLOCATOR_H_

#include <string>
#include "ramp_builder.h"

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
    RampAllocator *pool_;

public:
    // Constructor.
    SAllocator(RampAllocator *pool) noexcept : pool_(pool) {}

    SAllocator() noexcept : debug_level(3), pool_(NULL) {}

    template <class U>
    SAllocator(SAllocator<U> const& a) noexcept :
        pool_(a.pool_) {}

    pointer allocate(size_type num) {
        if (pool_ == NULL) { return this->temp_allocate(num); }
        return static_cast<pointer>(
            pool_->allocate(num * sizeof(value_type)));
    }
    void deallocate(pointer addr, size_type num) noexcept {
        if (pool_ == NULL) { return this->temp_deallocate(addr, num); }
        return pool_->deallocate(addr, num * sizeof(value_type));
    }

    pointer temp_allocate(size_type num) {
        size_t bytes = num * sizeof(value_type);
        if (debug_level < 2) {
            std::cerr << "WARNING: calling malloc(size = " << bytes
                      << ") via default SAllocator()." << std::endl;
        }
        return static_cast<pointer>(malloc(bytes));
    }
    void temp_deallocate(pointer addr, size_type num) noexcept {
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

typedef basic_string<char, std::char_traits<char>, SAllocator<char>> rString;

#endif // __SALLOCATOR_H__