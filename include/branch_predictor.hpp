#ifndef _SJTU_PREDICTOR_HPP_
#define _SJTU_PREDICTOR_HPP_
#include "instruction.hpp"
#include <cstring>

/**
 * @brief local 2 bit saturating counter
 */
class BranchPredictor {
private:
  unsigned char table[1 << 7];

public:
  BranchPredictor() { memset(table, 1, sizeof(table)); }
  bool predict(const Instruction &ins, unsigned address) const {
    if (ins.type == JALR)
      return 0;
    if (ins.type == JAL)
      return 1;
    return table[getRange<14, 7>(address)] >> 1;
  }
  void update(const Instruction &ins, unsigned address, bool jump) {
    if (ins.type == JAL || ins.type == JALR)
      return;
    unsigned hash = getRange<14, 7>(address);
    if (jump && table[hash] < 0b11)
      table[hash]++;
    if (!jump && table[hash] > 0b00)
      table[hash]--;
  }
};

#endif