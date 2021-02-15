#include "footprint.hh"

void instruLOAD(void* p, int size)
{
  uint64_t addr = reinterpret_cast<std::uintptr_t>(p);
  footprint_profile.instruLOAD(addr, size);
}

void instruSTORE(void* p, int size)
{
  uint64_t addr = reinterpret_cast<std::uintptr_t>(p);
  footprint_profile.instruSTORE(addr, size);
}
