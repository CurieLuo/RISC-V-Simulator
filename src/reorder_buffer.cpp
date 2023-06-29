#include "reorder_buffer.hpp"
#include "control_unit.hpp"
#include "load_store_buffer.hpp"
#include "reservation_station.hpp"

void ReorderBuffer::broadcast(ReservationStation &RS, LoadStoreBuffer &LSB,
                              int dest) {
  RS.receive(dest, a[dest].value);
  LSB.receive(dest, a[dest].value);
}

void ReorderBuffer::commit(ControlUnit &CU, ReservationStation &RS,
                           LoadStoreBuffer &LSB, RegisterFile &reg,
                           Memory &mem) {
  if (empty() || !front().ready)
    return;
  ReorderBufferEntry ins = front();
  int idx = head;
  pop();
  InstructionType type = ins.type;
  unsigned rd = ins.rd, value = ins.value, value2 = ins.value2,
           address = ins.address;
  if (type == END) {
    CU.end_ = 1;
    return;
  } //!!!!
  if (rd) {
    if (reg.depend[rd] == idx)
      reg.depend[rd] = -1; //!!!!!!!! next?
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
  // cerr << "commit: " << instructionName[type] << ' ' << std::hex << address<<
  // endl; getchar(); //!!!!!!!!!!!!!
  if (isJumpOrBranch(type)) {
    if (type == JALR || isBType(type) && ins.prediction != value) {
      CU.stall_ = 0;
      LSB.stall_ = 0;
      clear();
      RS.clear();
      LSB.clear();
      reg.clearDependence();
      if (type == JALR) {
        CU.PC = value2; //!!!
      } else {
        CU.PC = address + (value ? ins.imm : 4);
      }
    }
  }
}