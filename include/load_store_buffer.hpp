#ifndef _SJTU_LD_ST_BUFF_HPP_
#define _SJTU_LD_ST_BUFF_HPP_

#include "instruction.hpp"
#include "memory.hpp"
#include "queue.hpp"
#include "register.hpp"

class ReorderBuffer;
class ControlUnit;
class ReservationStation;

struct LoadStoreBufferEntry {
public:
  unsigned value1, value2;
  int depend1, depend2;
  int dest; // RoB index
  int clock;

public:
  LoadStoreBufferEntry() = default;
  LoadStoreBufferEntry(ReorderBuffer &RoB, RegisterFile &reg, unsigned rs1,
                       unsigned rs2, int d);
};

struct LoadStoreBuffer : public Queue<LoadStoreBufferEntry, 16> {
public:
  bool stall_ = 0;

public:
  void issue(ReorderBuffer &RoB, RegisterFile &reg, const Instruction &ins,
             int dest) {
    push(LoadStoreBufferEntry(RoB, reg, ins.rs1, ins.rs2, dest));
  }
  void receive(int dest, unsigned value);
  void execute(ReorderBuffer &RoB, ReservationStation &RS, Memory &mem);
};

#endif