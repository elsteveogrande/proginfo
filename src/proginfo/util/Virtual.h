#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Alloc.h"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <utility>

namespace proginfo::util {

template <class B>
class Virtual {
  static_assert(std::has_virtual_destructor_v<B>);

  B* ptr_ {nullptr};
  unsigned size_ {0};

  void clear() {
    if (ptr_) {
      ptr_->~B();
      Alloc::get().dealloc(ptr_);
      ptr_ = nullptr;
    }
  }

  Virtual& moveFrom(Virtual&& rhs) {
    if (&rhs != this) {
      clear();
      if (rhs.ptr_) { std::swap(ptr_, rhs.ptr_); }
    }
    return *this;
  }

public:
  ~Virtual() { clear(); }

  Virtual() = default;

  Virtual(Virtual const& rhs) = delete;
  Virtual& operator=(Virtual const& rhs) = delete;

  Virtual(Virtual&& rhs) { moveFrom(std::move(rhs)); }
  Virtual& operator=(Virtual&& rhs) { return moveFrom(std::move(rhs)); }

  Virtual<B>& check() {
    if (!*this) {
      std::cerr << "Check failed: " << (typeid(*this).name()) << " (empty)\n";
      abort();
    }
    return *this;
  }

  template <class T, class... A>
  void emplace(A&&... args) {
    static_assert(std::has_virtual_destructor_v<T>);
    static_assert(std::is_base_of_v<B, T>);
    clear();
    size_ = sizeof(T);
    ptr_ = (T*) Alloc::get().alloc(size_);
    assert(ptr_);
    new (ptr_) T(std::forward<A>(args)...);
  }

  B* operator->() { return ptr_; }

  B& operator*() {
    assert(ptr_);
    return *ptr_;
  }

  operator bool() const { return ptr_; }
};

}  // namespace proginfo::util
