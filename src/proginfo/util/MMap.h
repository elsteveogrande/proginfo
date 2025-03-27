#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string_view>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

namespace proginfo::util {

struct MMap {
  std::string_view path_;
  int fd_;
  size_t size_ {};
  void const* addr_ {};
  int errno_ {};

  MMap& check() {
    if (!*this) {
      if (errno_) {
        std::cerr << "MMap check failed: " << strerror(errno_) << '\n';
        abort();
      }
    }
    return *this;
  }

  explicit MMap(std::string_view path) : path_(path) {
    fd_ = open(path.data(), O_RDONLY);
    if (fd_ < 0) {
      errno_ = errno;
      return;
    }

    off_t seekRes = lseek(fd_, 0, SEEK_END);
    if (seekRes < 0) {
      errno_ = errno;
      close(fd_);
      return;
    }
    size_ = size_t(seekRes);

    auto* mmapRes = mmap(nullptr, size_, PROT_READ, MAP_SHARED, fd_, 0);
    if (mmapRes == MAP_FAILED) {
      errno_ = errno;
      close(fd_);
      return;
    }
    addr_ = mmapRes;
  }

  ~MMap() {
    if (addr_ && size_) { munmap((void*) addr_, size_); }
    if (fd_) { close(fd_); }
  }

  operator bool() const { return addr_ && size_; }
};

}  // namespace proginfo::util
