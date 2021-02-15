#ifndef __FOOTPRINT_HH__
#define __FOOTPRINT_HH__

#include <cstdint>
#include <iostream>
#include <map>

class MemoryFootprint
{
  public:
    typedef uint64_t UINT64;

  public:
    MemoryFootprint() 
    { 
      std::cout << "[LLVM Profiler: MemoryFootprint object created] \n";
    }

    void instruLOAD(UINT64 addr, int size)
    {
      UINT64 base = addr;

      std::map<UINT64,unsigned int>::iterator iter = mem_loc.find(base);
      if (iter == mem_loc.end())
      {
        mem_loc[base] = LOAD;
      }
      else
      {
        mem_loc[base] = iter->second | LOAD;
      }

      if (summary_cnt++ == summary_cnt_th)
      {
        summary();
        summary_cnt = 0;
      }
    }

    void instruSTORE(UINT64 addr, int size)
    {
      UINT64 base = addr;

      std::map<UINT64,unsigned int>::iterator iter = mem_loc.find(base);
      if (iter == mem_loc.end())
      {
        mem_loc[base] = STORE;
      }
      else
      {
        mem_loc[base] = iter->second | STORE;
      }

      if (summary_cnt++ == summary_cnt_th)
      {
        summary();
        summary_cnt = 0;
      }
    }

    void summary()
    {
      std::cout << "\n-----------------Summary----------------\n";
      for (unsigned i = 0; i < num_mem_loc_types; i++)
      {
        mem_loc_stats[i] = 0;
      }
      std::map<UINT64,unsigned int>::iterator iter = mem_loc.begin();
      for (; iter != mem_loc.end(); iter++)
      {
        mem_loc_stats[iter->second]++;
      }

      for (unsigned i = 1; i < num_mem_loc_types; i++)
      {
        std::cout << mem_loc_types[i] << ": " 
                  << mem_loc_stats[i] << " doublewords\n";
      }
      std::cout << "----------------------------------------\n";
    }
  protected:
    const unsigned summary_cnt_th = 500000;
    unsigned summary_cnt = 0;

  protected:
    const unsigned int LOAD = 1;
    const unsigned int STORE = 2;

    // All the touched memory locations - 
    //   Byte-addressable or Page-addressable
    std::map<UINT64, unsigned int> mem_loc;

    // num_mem_loc_types - number of mem. loc. types
    //   A memory location in mem_loc can have 4 types
    //     0 - invalid
    //     1 - load (touched by a load instruction)
    //     2 - store (touched by a store instruction)
    //     3 - load+store (touched by load and store instructions)
    static const unsigned num_mem_loc_types = 4;

    UINT64 mem_loc_stats[num_mem_loc_types];
    const char* mem_loc_types[num_mem_loc_types] = {"error",
                                                    "load",
                                                    "store",
                                                    "load+store"
    };
};

static MemoryFootprint footprint_profile;

#endif
