#ifndef _SJTU_ROB_HPP_
#define _SJTU_ROB_HPP_

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

struct ReorderBuffer : public Queue<ReorderBufferEntry, 32> {

public:
  int getCurrentIndex() const { return tail; }
  inline void issue(RegisterFile &reg, const Instruction &ins, unsigned address,
                    bool prediction) {
    if (ins.rd)
      reg.depend[ins.rd] = getCurrentIndex();
    push(ReorderBufferEntry(ins, address, prediction));
  }
  void broadcast(ReservationStation &RS, LoadStoreBuffer &LSB, int dest);
  void commit(ControlUnit &CU, ReservationStation &RS, LoadStoreBuffer &LSB,
              RegisterFile &reg, Memory &mem);
};

#endif