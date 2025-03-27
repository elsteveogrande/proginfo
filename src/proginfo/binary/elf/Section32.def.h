#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "Section32.h"
#include "Segment32.h"

namespace proginfo::binary {

inline Section32::Section32(Binary const* bin, util::Addr addr)
    : Section(bin, addr) {}

inline util::Virtual<Segment> Section32::segment() const {
  util::Virtual<Segment> ret;
  bin().eachSegment([&](Segment const& seg) {
    if (seg.virtAddr() <= virtAddr() &&
        virtAddr() < seg.virtAddr() + seg.virtSize()) {
      ret.emplace<Segment32>((Segment32 const&) seg);
      return false;
    }
    return true;
  });
  return ret;
}

}  // namespace proginfo::binary
