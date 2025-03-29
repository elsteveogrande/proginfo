#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Binary.h"
#include "proginfo/binary/Segment.h"

namespace proginfo::binary {

inline bool Segment::isLE() const { return bin_->isLE(); }

}  // namespace proginfo::binary
