#ifndef _SJTU_CONTROL_UNIT_HPP_
#define _SJTU_CONTROL_UNIT_HPP_

#include "branch_predictor.hpp"
#include "instruction.hpp"
#include "memory.hpp"
#include "register.hpp"

class ReservationStation;
class LoadStoreBuffer;
class ReorderBuffer;

class ControlUnit {
  friend class ReorderBuffer;

private:
  unsigned clk = 0; // clock
  unsigned PC = 0;  // program counter
  bool stall_ = 0;  // end of program / wrong prediction
  bool end_ = 0;    // commit END
public:
  void update(ReservationStation &RS, LoadStoreBuffer &LSB, ReorderBuffer &RoB,
              RegisterFile &reg);
  void fetchInstruction(ReservationStation &RS, LoadStoreBuffer &LSB,
                        ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
                        BranchPredictor &BP);
  bool end() const { return end_; }

public:
  void run(ReservationStation &RS, LoadStoreBuffer &LSB, ReorderBuffer &RoB,
           RegisterFile &reg, Memory &mem, BranchPredictor &BP);
};

#endif