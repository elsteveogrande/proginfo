#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include <functional>

namespace proginfo::util {

struct Cleanup {
  std::function<void()> func_;
  explicit Cleanup(std::function<void()> func) : func_(func) {}
  ~Cleanup() { func_(); }
};

};  // namespace proginfo::util
