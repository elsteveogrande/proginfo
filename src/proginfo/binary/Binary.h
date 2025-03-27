#pragma once
static_assert(__cplusplus >= 201700L, "proginfo requires C++17");

#include "detail/Binary.h"
#include "elf/ELF.h"
#include "elf/ELF32.h"
#include "elf/ELF64.h"
#include "elf/Section32.h"
#include "elf/Section64.h"
#include "elf/Segment32.h"
#include "elf/Segment64.h"
#include "elf/Symbol32.h"
#include "elf/Symbol64.h"
#include "elf/SymbolTable32.h"
#include "elf/SymbolTable64.h"

// 'def' files go last

#include "detail/Binary.def.h"
#include "elf/ELF32.def.h"
#include "elf/ELF64.def.h"
#include "elf/Section32.def.h"
#include "elf/Section64.def.h"
#include "elf/SymbolTable32.def.h"
#include "elf/SymbolTable64.def.h"
