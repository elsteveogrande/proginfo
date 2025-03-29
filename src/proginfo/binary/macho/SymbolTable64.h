#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "../../util/Virtual.h"
#include "../Section.h"
#include "../Segment.h"
#include "../SymbolTable.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

namespace proginfo::binary {}
