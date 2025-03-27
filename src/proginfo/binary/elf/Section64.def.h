#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Section64.h"
#include "Segment64.h"

namespace proginfo::binary {

inline util::Virtual<Segment> Section64::segment() const {
  util::Virtual<Segment> ret;
  bin().eachSegment([&](Segment const& seg) {
    if (seg.virtAddr() <= virtAddr() &&
        virtAddr() < seg.virtAddr() + seg.virtSize()) {
      ret.emplace<Segment64>((Segment64 const&) seg);
      return false;
    }
    return true;
  });
  return ret;
}

}  // namespace proginfo::binary
