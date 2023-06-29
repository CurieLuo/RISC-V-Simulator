#ifndef _SJTU_CONTROL_UNIT_HPP_
#define _SJTU_CONTROL_UNIT_HPP_

#include "instruction.hpp"
#include "memory.hpp"
#include "predictor.hpp"
#include "register.hpp"

class ReservationStation;
class LoadStoreBuffer;
class ReorderBuffer;

struct ControlUnit {
public:
  unsigned clk = 0; // clock
  unsigned PC = 0;  // program counter
public:
  bool stall_ = 0; // end of program / wrong prediction
  bool end_ = 0;

public:
  void update(ReservationStation &RS, LoadStoreBuffer &LSB, ReorderBuffer &RoB,
              RegisterFile &reg) {
    reg.update();
  }
  void fetchInstruction(ReservationStation &RS, LoadStoreBuffer &LSB,
                        ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
                        BranchPredictor &BP);

public:
  bool run(ReservationStation &RS, LoadStoreBuffer &LSB, ReorderBuffer &RoB,
           RegisterFile &reg, Memory &mem, BranchPredictor &BP);
};

#endif