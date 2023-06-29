#ifndef _SJTU_MEMORY_HPP_
#define _SJTU_MEMORY_HPP_

#include "instruction.hpp"

struct Memory {
private:
  static constexpr int MEM_SZ = 1 << 22;
  unsigned char mem[MEM_SZ];

public:
  template <unsigned size> inline unsigned read(unsigned begin) {
    unsigned ret = 0;
    for (int i = 0; i < size; i++) {
      ret |= mem[begin + i] << (i * 8);
    }
    return ret;
  }
  template <unsigned size> inline unsigned readSigned(unsigned begin) {
    return signExtend<size * 8>(read<size>(begin));
  }
  template <unsigned size> inline void write(unsigned begin, unsigned value) {
    for (unsigned p = begin, end = begin + size; p < end; p++) {
      mem[p] = value & 255u;
      value >>= 8;
    }
  }
};

#endif