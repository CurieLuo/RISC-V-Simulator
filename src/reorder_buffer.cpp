#include "reorder_buffer.hpp"
#include "control_unit.hpp"
#include "load_store_buffer.hpp"
#include "reservation_station.hpp"

void ReorderBuffer::commit(ControlUnit &CU, ReservationStation &RS,
                           LoadStoreBuffer &LSB, RegisterFile &reg, Memory &mem,
                           BranchPredictor &BP) {
  if (cur.empty() || !cur.front().ready)
    return;
  const ReorderBufferEntry &ins = cur.front();
  int idx = getHead();
  next.pop();
  InstructionType type = ins.type;
  if (type == END) {
    CU.end_ = 1;
    return;
  }
  unsigned rd = ins.rd, value = ins.value, value2 = ins.value2,
           address = ins.address;
  if (rd) {
    if (reg.depend_next[rd] == idx)
      reg.depend_next[rd] = -1;
    //!!! must use the value from the new clock cycle
    reg.next[rd] = value;
  } else if (isSType(type)) {
    LSB.stall_ = 0;
    switch (type) {
    case SB:
      mem.write<1>(value, getRange<8, 0>(value2));
      break;
    case SH:
      mem.write<2>(value, getRange<16, 0>(value2));
      break;
    case SW:
      mem.write<4>(value, getRange<32, 0>(value2));
      break;
    }
  }
  if (isJumpOrBranch(type)) {
    BP.update(ins, address, bool(value));
    if (type == JALR || isBType(type) && ins.prediction != value) {
      CU.stall_ = 0;
      LSB.stall_ = 0;
      next.clear();
      RS.clear();
      LSB.next.clear();
      reg.clearDependence();
      CU.update(RS, LSB, *this, reg); // clear all
      if (type == JALR) {
        CU.PC = value2;
      } else {
        CU.PC = address + (value ? ins.imm : 4);
      }
    }
  }
}