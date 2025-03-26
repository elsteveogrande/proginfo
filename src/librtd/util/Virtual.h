#pragma once
static_assert(__cplusplus >= 201700L, "librtd requires C++17");

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>

namespace librtd::util {

template <class B>
class Virtual {
  static_assert(std::has_virtual_destructor_v<B>);

  constexpr static size_t kSpace = 4;

  std::byte space_[kSpace] {};
  B* ptr_ {nullptr};

public:
  ~Virtual() {
    if (ptr_) {
      delete ptr_;
      ptr_ = nullptr;
    }
  }

  Virtual() = default;

  template <class T, class... A>
  void emplace(A... args) {
    static_assert(std::has_virtual_destructor_v<T>);
    static_assert(std::is_base_of_v<B, T>);
    auto* ptr = (B*) &space_;
    size_t space = kSpace;
    std::align(alignof(T), sizeof(T), (void*&) ptr, space);
    assert(ptr);
    new (ptr) T(std::forward<A>(args)...);
    ptr_ = ptr;
  }

  B* operator->() { return ptr_; }

  B& operator*() {
    assert(ptr_);
    return *ptr_;
  }

  operator bool() const { return ptr_; }
};

}  // namespace librtd::util
