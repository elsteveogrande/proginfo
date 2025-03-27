#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory_resource>
#include <new>

namespace proginfo::util {

// clang-format off

/*
Reserve a small-ish hunk of memory on the stack and use that for `proginfo`'s
allocation operations.

We use a number of classes which *generally* are pretty small (e.g. just a pointer
to some address).  These classes have some virtual inheritance, and they need to
be passed around by pointer or reference (pointers to variably-sized blocks).
Usually you can say `BaseT* base = new DerivedT(...)` and call it a day but since
we have to avoid heap allocations, we'll rely on this `Alloc` instead.

This allocator will claim up-front a modest amount of space on the stack and have
it behave like a "mini-heap" of limited size.  We would generally have only a few
small objects in our "heap" at any given time, so this shouldn't require a lot of
scratch space; a few KB at most.  Since this class takes up residence on the stack,
it's not movable nor copyable.  It's only `equal_to` itself, i.e. if `&a1 == &a2`.

Only one `Alloc` should be present in some given scope.  For convenience, this one
instance makes itself available via a static thread-local ptr so it doesn't need to
be passed through the call chain.

Limitation: this will not work in a multi-threaded context, i.e. if an `Alloc` is
created by a thread T and is living on the stack, it won't automatically be visible
in threads subsequently _spawned by_ T.

This is a `std::pmr::memory_resource`, so it's easily usable with `std::pmr::allocator`.

FIXME: Probably worth profiling.
*/

class Alloc : public std::pmr::memory_resource {
  // Populated when a thread creates an Alloc, and nulled out
  // when the Alloc is being destructed.  In effect this is a context-based
  // RAII-style allocation that is available at any time until the context
  // is discarded; one can use `get()` to access this instance if they won't
  // want to pass this object through every call.
  static thread_local Alloc* instance_;

  // Enable to collect stats and dump them when destroying instance.
  // NOTE: this prints stuff using `fprintf` and is not signal-safe.
  constexpr static bool kDebug = false;

public:
  static Alloc& get() {
    assert(instance_);
    return *instance_;
  }

private:
  // Allocate in units of this size
  constexpr static unsigned kSlotSize = alignof(std::max_align_t);

  // Total size of this storage in bytes
  constexpr static unsigned kSize = 640;
  static_assert(!(kSize % kSlotSize));

  // Used if `kDebug` enabled
  unsigned allocs_{0};
  unsigned deallocs_{0};
  unsigned allocSlots_{0};
  unsigned deallocSlots_{0};
  unsigned highMarkAllocs_{0};
  unsigned highMarkSlots_{0};
  void recordAlloc(unsigned slots);
  void recordDealloc(unsigned slots);
  void checkStats();

  // We have this many slots to work with.
  constexpr static unsigned kSlots = kSize / kSlotSize;

  // In some places a slot index is represented with a uint16_t; avoid overflow
  static_assert(kSlots < 65536);

  // Each allocation of size S will occupy:
  //   * ceil(S / kSlotSize) slots as contiguous byte storage
  //   * plus one slot acting as a block header for that allocation

  // Minimum threshold for this to be useful (arbitrary)
  static_assert(kSlots >= 10);

  // Slots are assigned like so:
  // for some allocation, slot[P] is the slot describing the allocation
  //   * slots[P]:     Slot(type, S), where type is F or U (free or used),
  //                   and S represents size of allocation, in slots; this >= 1
  //   * slots[p+1]... Free or Used space
  //   * slots[p+1+S]: Slot describing the next allocation
  //
  // Initially an `Alloc` object having K total slots looks like:
  //   * slots[0]      Slot(type='F', size=K-1)
  //                   i.e. one free chunk occupying (nearly) all space
  //
  // After an allocation of size S; kSlotSize < S <= kSlotSize * 2 (i.e., two slots of space)
  //   * slots[0]      Slot(type='U', size=2)
  //   * slots[1]...   space for the allocated object of at least S bytes
  //   * slots[3]      Slot(type='F', size=K-4)
  //
  // The slots are singly-"chained" in order starting at slot 0.
  // Notice that every allocation consumes a small amount of space describing it
  // (one slot's worth).  We alleviate this somewhat by the `coalesce` operation (see below).
  //
  // When deallocating a given pointer X, we'll locate the header for that allocation
  // which starts at (X - kSlotSize), i.e. the slot just before the allocated bytes.
  // Then we'll simply mark that chunk of memory free (type 'F').
  //
  // Since this allocation and deallocation strategy (1) consumes a slot for each
  // free or used chunk of memory, and (2) results in fragmentation, we will try to
  // occasionally coalesce free blocks: find consecutive adjacent free-chunks and
  // merge them.
  //
  // When deallocating we'll try twice to coalesce free blocks.  This is limited to
  // two attempts simply to balance housekeeping operations vs. efficiency with respect
  // to space.  If an allocation fails we'll aggressively coalesce-and-retry until it succeeds.

  struct Slot {
    char type;      // F(ree) or U(sed)
    uint16_t size;  // free or used slots following this marker
    std::byte _pad[kSlotSize - 4];  // The above total 4 bytes (including alignment).
  };
  static_assert(sizeof(Slot) == kSlotSize);

  // Together these slots form our memory space.
  // We won't initialize these bytes in the interest of fast init / destruction.
  Slot slots_[kSlots];

  // We use this invalid "index" as a special value indicating failure.
  constexpr static unsigned kInvalid = kSlots;

  // When linear-searching for free blocks, start here (instead of at 0).
  unsigned firstFree_ {kInvalid};

public:
  virtual ~Alloc() {
    // "Un-install" self as the thread-local context's alloc
    assert(instance_ == this);
    instance_ = nullptr;
    checkStats();  // dump stats if debugging; also check alloc/dealloc count, abort if wrong
  }

  Alloc() {
    // Ensure none exists in (thread-local) context
    assert(!instance_);
    // Initialize only the first entry as free space
    slots_[0] = {.type = 'F', .size = kSlots - 1, ._pad = {}};
    // Initially first free block (and only free block) is at 0.
    firstFree_ = 0;
    // "Install" self as thread's allocator (until destruction)
    instance_ = this;
  }

  // We want this thing to be parked in a predictable place on the stack,
  // so no copying or moving is allowed.
  Alloc(Alloc const&) = delete;
  Alloc(Alloc&&) = delete;
  Alloc& operator=(Alloc const&) = delete;
  Alloc& operator=(Alloc&&) = delete;

  // For std::pmr::memory_resource
  bool do_is_equal(std::pmr::memory_resource const& rhs) const noexcept override {
    // These two are "equal" IFF they are the same object, because an Alloc
    // instance owns a certain region of (stack-allocated) space, and two different
    // Allocs cannot be mixed/matched for de/allocations.
    return (&rhs == this);
  }

  // For std::pmr::memory_resource.
  // Same as `alloc` below, but throws `bad_alloc` instead of returning nullptr if failed.
  // Doesn't support large alignments, i.e. fails if align is > `kSlotSize`.
  [[nodiscard]] void* do_allocate(size_t size, size_t align) override {
    if (align > kSlotSize) { throw std::bad_alloc(); }
    if (size > kSize) { throw std::bad_alloc(); }
    auto ret = (void*) alloc(unsigned(size));
    if (ret == nullptr) { throw std::bad_alloc(); }
    return ret;
  }

  // For std::pmr::memory_resource.
  // `size` and `align` are not useful here, but they are checked for unreasonable values.
  void do_deallocate(void* ptr, size_t size, size_t align) noexcept override {
    assert (align <= kSlotSize);
    assert (size <= kSize);
    dealloc(ptr);
  }

  // Wrap self in a `std::pmr::allocator`.
  template <typename T>
  std::pmr::polymorphic_allocator<T> pmrAlloc() { return {this}; }

  [[nodiscard]] std::byte* alloc(unsigned size) {
    if (size > kSize) { return nullptr; }             // Check within reason
    if (!size) { ++size; }                            // Prevent zero-size allocations
    auto slots = (size + kSlotSize - 1) / kSlotSize;  // Determine number of slots, rounding up
retry:
    auto index = claimSpace(unsigned(slots));         // Find first free slot;
    if (index == kInvalid) {                          // invalid index means no space.
      if (coalesce()) { goto retry; }                 // If we could coalesce any free slots, retry.
      return nullptr;                                 // Otherwise if we couldn't coalesce; fail.
    }
    auto* ret = &slots_[index + 1];                   // Success, return space just after this slot.
    recordAlloc(slots);
    return (std::byte*) ret;
  }

  /** Deallocate the memory at this pointer, which must have been returned from a prior call to`alloc`. */
  void dealloc(void* _obj) {
    auto thisP = uintptr_t(_obj) - kSlotSize;         // Object's slot "header" immediately precedes object
    auto& thisM = *(Slot*) (thisP);                   // Get location of this header-slot,
    assert (thisM.type == 'U');                       // verify it's an allocated obj.
    assert (thisP >= uintptr_t(&slots_));             // Range check: ensure this space belongs to us
    assert (thisP < uintptr_t(slots_ + kSlots));      // Range check: ensure this space belongs to us
    assert(!(thisP % kSlotSize));                     // (Should be aligned)
    thisM.type = 'F';                                 // Mark this chunk as free,
    recordDealloc(thisM.size);                        // do bookkeeping,
    coalesce();                                       // and finally coalesce if possible.
  }

private:
  /** Search the structures in our space for a free block of sufficient size. */
  unsigned claimSpace(unsigned slots) {
    unsigned thisI = firstFree_ % kSlots;             // Starting at first [free, if known] block, search through
    while (thisI < kSlots) {                          // each allocation, stopping at end of space.
      auto& thisM = slots_[thisI];                    // At this slot,
      if (thisM.type == 'F' && thisM.size >= slots) { // if this is free space, and it's big enough,
        splitFree(thisI, slots);                      // then carve out space that we need
        return thisI;                                 // and return successfully to caller (`alloc`).
      }
      thisI = thisI + thisM.size + 1;                 // Otherwise keep searching ...
    }
    return kInvalid;                                  // Failed
  }

  /** Given this free space block, split it into a "used" block for the allocation of
  `slots` worth of space, and a "free" block with the remainder, if there's enough. */
  void splitFree(unsigned thisI, unsigned slots) {
    auto& thisM = slots_[thisI];                      // At the given free space block,
    assert(thisM.size >= slots);                      // we should have enough space for allocation;
    if (thisM.size < slots + 2) {                     // but if this is isn't quite big enough to split,
      thisM.type = 'U';                               // we'll just claim this whole block.
      if (firstFree_ == thisI) {                      // If this block was the first free, block,
        firstFree_ = kInvalid;                        // then invalidate it, because we don't know where next is
      }
      return;
    }
    auto nextI = unsigned(thisI + slots + 1);         // A fter the chunk at `thisI` we'll make
    auto& nextM = slots_[nextI];                      // a new "free" chunk.
    nextM.type = 'F';                                 // The slot at `nextM` describes this new free block,
    nextM.size = uint16_t(thisM.size - slots - 1);    // which  now shrunk by `slots+1` in size
    thisM.type = 'U';                                 // and this slot at `thisI` is now used
    thisM.size = uint16_t(slots);                     // in an allocation of this size.
    firstFree_ = std::min(firstFree_, thisI);
  }

  /** Merge adjacent free blocks, stopping after merging the first-found pair of free blocks. */
  bool coalesce() {
    unsigned thisI = firstFree_ % kSlots;             // Starting at first [free, if known] block,
    while (thisI != kSlots) {                         // we'll scan the entire space for free blocks
      assert(thisI < kSlots);                         // (thisI shouldn't have exceeded this area)
      auto& thisS = slots_[thisI];                    // We'll examine this slot,
      auto nextI = thisI + thisS.size + 1;            //  and the one after it, for mergeable free space.
      if (thisS.type != 'F') {                        // If this isn't free space, skip.
        thisI = nextI;                                // Move on to next block.
        continue;
      }
      if (nextI == kSlots) { return false; }          // If it's at the end, we're done; indicate no merges.
      assert(nextI < kSlots);                         // (nextI shouldn't have exceeded this area)
      auto& nextS = slots_[nextI];                    // Otherwise, at the next slot,
      if (nextS.type != 'F') {                        // if we don't see free space, then
        thisI = nextI + nextS.size + 1;               // move right along; skip `this` _and_ `next`.
        continue;
      }
      thisS.size = thisS.size + nextS.size + 1;       // Othe rwise, we can merge; enlarge the earlier freeblock.
      firstFree_ = std::min(firstFree_, thisI);       // If this is the earliest free block, update that.
      return true;                                    // Indicate we made some progress on merging.
    }
    return false;
  }
};

inline thread_local Alloc* Alloc::instance_ {nullptr};

inline void Alloc::recordAlloc(unsigned slots) {
  allocs_++;
  if (!kDebug) { return; }
  allocSlots_ += slots;
  auto active = allocs_ - deallocs_;
  auto activeSlots = allocSlots_ - deallocSlots_;
  highMarkAllocs_ = std::max(highMarkAllocs_, active);
  highMarkSlots_ = std::max(highMarkSlots_, activeSlots);
}

inline void Alloc::recordDealloc(unsigned slots) {
  deallocs_++;
  if (!kDebug) { return; }
  deallocSlots_ += slots;
  auto active = allocs_ - deallocs_;
  auto activeSlots = allocSlots_ - deallocSlots_;
  highMarkAllocs_ = std::max(highMarkAllocs_, active);
  highMarkSlots_ = std::max(highMarkSlots_, activeSlots);
}

inline void Alloc::checkStats() {
  // Any outstanding objects which were never deallocated?
  // Then fail noisily and immediately; this is almost certainly a problem
  // since something is still referring to our memory, which is about to
  // go out of scope.
  if (allocs_ != deallocs_) {
    fprintf(stderr,
            "util::Alloc destroyed with %d objects still in use\n",
            (allocs_ - deallocs_));
    // Abort now, unless debugging; the ensuing ASAN crash will probably help us.
    if (!kDebug) { abort(); }
  }
  
  // Dump this stuff out, only if debugging.
  if (!kDebug) { return; }

  auto totalUsed = (highMarkAllocs_ + highMarkSlots_) * kSlotSize;
  auto totalUtil = float(totalUsed) / kSize;
  printf("### allocs:%4d deallocs:%4d allocSlots:%4d deallocSlots:%4d "
         "highMarkAllocs:%4d highMarkSlots:%4d; "
         "peak usage %d bytes (%d%%)\n",
         allocs_, deallocs_, allocSlots_, deallocSlots_,
         highMarkAllocs_, highMarkSlots_, 
         totalUsed,
         int(std::roundf(100.0f * totalUtil)));
}

}  // namespace proginfo::util
