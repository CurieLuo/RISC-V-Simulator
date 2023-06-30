#ifndef _SJTU_ROB_HPP_
#define _SJTU_ROB_HPP_

#include "branch_predictor.hpp"
#include "instruction.hpp"
#include "memory.hpp"
#include "queue.hpp"
#include "register.hpp"

class ControlUnit;
class ReservationStation;
class LoadStoreBuffer;

struct ReorderBufferEntry : public Instruction {
public:
  bool ready = 0;      // ready to commit
  bool prediction = 0; // record prediction for branch instructions
  unsigned value = 0;
  unsigned value2 = 0;
  unsigned address;

public:
  ReorderBufferEntry() = default;
  ReorderBufferEntry(const Instruction &i, unsigned a, bool p = 0)
      : Instruction(i), address(a), prediction(p), ready(isSimple(type)) {
    switch (type) {
    case LUI:
      value = imm;
      break;
    case AUIPC:
      value = address + imm;
      break;
    case JAL:
    case JALR:
      value = address + 4;
      break;
    default:
      break;
    }
  }
};

class ReorderBuffer {
  friend class ControlUnit;
  friend class LoadStoreBuffer;
  friend class ReservationStation;

private:
  Queue<ReorderBufferEntry, 32> cur, next;

public:
  const ReorderBufferEntry &operator[](int idx) const { return cur[idx]; }
  void update() { cur = next; }
  int getTail() const { return next.tail; }
  int getHead() const { return next.head; }
  bool full() const { return cur.full(); }
  bool empty() const { return cur.empty(); }
  inline void issue(RegisterFile &reg, const Instruction &ins, unsigned address,
                    bool prediction) {
    if (ins.rd)
      reg.depend_next[ins.rd] = getTail();
    next.push(ReorderBufferEntry(ins, address, prediction));
  }
  void commit(ControlUnit &CU, ReservationStation &RS, LoadStoreBuffer &LSB,
              RegisterFile &reg, Memory &mem, BranchPredictor &BP);
};

#endif