#ifndef _SJTU_PREDICTOR_HPP_
#define _SJTU_PREDICTOR_HPP_
#include "instruction.hpp"

struct BranchPredictor {
public:
  bool predict(const Instruction &ins) {
    if (ins.type == JALR)
      return 0;
    if (ins.type == JAL)
      return 1; //!
    return 1;   // TODO
  }
};

#endif