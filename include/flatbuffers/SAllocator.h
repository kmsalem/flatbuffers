#ifndef _SALLOCATOR_H_
#define _SALLOCATOR_H_

#include <string>
#include <algorithm>
#include <unordered_map>

/*
    Some helper function for buddy allocator
*/ 
#define CEIL_DIV(A, B) ((((A) - 1) / (B)) + 1)

static inline bool is_word_aligned(void *pv)
{ return ((uintptr_t)pv & 0x03) == 0; }

#define NotReachedReturn(...) \
	do \
	{ \
		assert(false); \
		return __VA_ARGS__; \
} while(0)

// can't cross byte boundary
static inline void SetBits(uint8_t *pb, size_t ibit, uint8_t mask, uint8_t val)
{
	// uint8_t ib = ibit >> 3;
    uint32_t ib = ibit >> 3;
	ibit = ibit % 8;
	pb[ib] = (pb[ib] & ~(mask << ibit)) | (val << ibit);
}

static inline uint8_t BitsGet(const uint8_t *pb, size_t ibit, uint8_t mask)
{
	// uint8_t ib = ibit >> 3;  // ib is limit to 8 bits???
    uint32_t ib = ibit >> 3;
	ibit = ibit % 8;
	return (pb[ib] >> ibit) & mask;
}

/*
    RampAlloc is actually a memory pool, 
      which stores relevant information about available memory
*/ 
class RampAlloc {
 public:

  explicit RampAlloc(void *addr, 
                     uint8_t cb2size,
                     uint8_t cb2Min):
            addr(addr),
            cb2size(cb2size),
            cb2Min(cb2Min) {
                assert(is_word_aligned(addr));
                size = 1 << cb2size;
                Min = 1 << cb2Min;
                // m_track = new (addr+sizeof(RampAlloc)) uint8_t[]; // placement new on array is different from on object
                m_track = (uint8_t *)addr+sizeof(RampAlloc);
                t_size = CEIL_DIV(1 << (cb2size - cb2Min + 1), 8);
                memset(m_track, 0, t_size);
                SetBits(m_track, (1 << (cb2size - cb2Min + 1)) - 2, 0b01, 0b01);  // this does not work correctly. should just set last bit to 1
            }
  
  void* allocate(size_t cb) noexcept {
    // do some error check first?
    if (cb > size)
        return nullptr;

    // how many blocks are needed?
    size_t cblkReq = 1;  // we want at least one smallest chunk
	while (cb > Min * cblkReq)
        cblkReq <<= 1;

    // find the smallest free block that will fit
	size_t iblkStart = 0;
	size_t iblkMin = 0;
	size_t cblkMin = 0;
	for (size_t iblk = 0; cblkMin != cblkReq && iblk < (1U << (cb2size - cb2Min)); )
	{
		switch (BitsGet(m_track, iblk << 1, 0b11))
		{
		case 0b00:
			++iblk;
			break;
		case 0b01:  // end of free block; accept if better
			if (iblk - iblkStart + 1 >= cblkReq && (cblkMin == 0 || cblkMin > (iblk - iblkStart + 1)))
			{
				cblkMin = iblk - iblkStart + 1;
				iblkMin = iblkStart;
			}
			// fall through
		case 0b10:  // in-use block
		case 0b11:  // end of in-use region
			iblkStart = iblk = (iblk & ~(cblkReq - 1)) + cblkReq;
			break;
		}
	}

	if (cblkMin == 0) {
        LogError("cannot find a big enough chunk");
		return nullptr;
    }

	// split it until it's the required size
	while (cblkMin > cblkReq)
	{
		cblkMin >>= 1;
		SetBits(m_track, (iblkMin + cblkMin - 1) << 1, 0b01, 0b01);
	}

    // Print out information for debug
    // LogInfo("allocate %zu blocks (%zu bytes needed) at %p", cblkReq, cb, (uint8_t *)addr + (iblkMin << cb2Min));
    // printf("%zu\n", cb);

	// mark in-use
	for (size_t iblk = iblkMin; iblk < iblkMin + cblkReq; ++iblk)
		SetBits(m_track, iblk << 1, 0b10, 0b10);
	
    return (uint8_t *)addr + (iblkMin << cb2Min);
  }

  void deallocate(void* pv, size_t bytes) noexcept {
    LogInfo("deallocate(addr = %p, bytes = %p)", pv, (void*) bytes);

    // find the block
	if (pv < addr || pv >= addr + size)
		NotReachedReturn();
	size_t iblk = ((uint8_t *)pv - (uint8_t *)addr) >> cb2Min;
	
	// mark the block as free
	size_t cblk = 0;
	for (size_t iblkMark = iblk; iblkMark < (1U << (cb2size - cb2Min)); ++iblkMark)
	{
		assert(BitsGet(m_track, (iblkMark << 1), 0b10) == 0b10);
		++cblk;
		SetBits(m_track, iblkMark << 1, 0b10, 0b00);
		if (BitsGet(m_track, iblkMark << 1, 0b01) == 0b01)
			break;
	}

	// coalesce with free buddies
	for ( ; cblk < 1U << (cb2size - cb2Min); cblk <<= 1)
	{
		// is the buddy free?
		size_t iblkBuddy = iblk ^ cblk;
		for (size_t iblkChk = iblkBuddy; iblkChk < iblkBuddy + cblk; ++iblkChk)
			if (BitsGet(m_track, (iblkChk << 1), 0b10) == 0b10)
				return;  // we're done

		// coalesce
		iblk = std::min(iblk, iblkBuddy);
		SetBits(m_track, (iblk + cblk - 1) << 1, 0b01, 0b00);
    }
  }

  // check the size actually used so far
  size_t CbUsed() const {
    size_t cblkUsed = 0;
        for (size_t iblk = 0; iblk < (1U << (cb2size - cb2Min)); ++iblk)
            if (BitsGet(m_track, iblk << 1, 0b10) == 0b10)
                ++cblkUsed;
        return cblkUsed * Min;
  }

  // get the size of bitmap
  size_t GetTsize() const {
      return t_size;
  }

  // get the size of least one piece segment size needed
  size_t GetSize() const {
    size_t max = 0;
    for (size_t iblk = 0; iblk < (1U << (cb2size - cb2Min)); ++iblk)
        if (BitsGet(m_track, iblk << 1, 0b10) == 0b10)
                max = iblk;
    max += 1;
    return max << cb2Min;
  }
private:
  void* addr;  // address of rdma memory region
  uint8_t cb2size; // power of the size as to base 2
  uint8_t cb2Min; // power of the mininal chunk size as to base 2
  size_t size; // size of the segment 
  size_t Min;  // size of the minimal chunk

  size_t t_size; // size of track
  uint8_t *m_track; // pointer to bitmap
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

/* Usage of string in RaMP */
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