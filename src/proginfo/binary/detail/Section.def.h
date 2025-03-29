#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Binary.h"
#include "proginfo/binary/Section.h"

namespace proginfo::binary {

inline bool Section::isLE() const { return bin_->isLE(); }

}  // namespace proginfo::binary
