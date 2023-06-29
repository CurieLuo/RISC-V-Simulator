#include "load_store_buffer.hpp"
#include "reorder_buffer.hpp"
#include "reservation_station.hpp"

LoadStoreBufferEntry::LoadStoreBufferEntry(ReorderBuffer &RoB,
                                           RegisterFile &reg, unsigned rs1,
                                           unsigned rs2, int d)
    : dest(d), value1(0), value2(0), clock(0) {
  depend1 = reg.depend[rs1];
  depend2 = reg.depend[rs2];
  if (depend1 == -1) {
    value1 = reg.cur[rs1];
  } else if (RoB[depend1].type == JALR || RoB[depend1].ready) {
    value1 = RoB[depend1].value;
    depend1 = -1;
  }
  if (depend2 == -1) {
    value2 = reg.cur[rs2];
  } else if (RoB[depend2].type == JALR || RoB[depend2].ready) {
    value2 = RoB[depend2].value;
    depend2 = -1;
  }
}

void LoadStoreBuffer::receive(int dest, unsigned value) {
  for (int i = head; i != tail; i = (i + 1) % N) {
    LoadStoreBufferEntry &entry = a[i];
    if (entry.depend1 == dest) {
      entry.depend1 = -1;
      entry.value1 = value;
    }
    if (entry.depend2 == dest) {
      entry.depend2 = -1;
      entry.value2 = value;
    }
  }
}

void LoadStoreBuffer::execute(ReorderBuffer &RoB, ReservationStation &RS,
                              Memory &mem) {
  if (empty() || stall_)
    return;
  LoadStoreBufferEntry &entry = front();
  if (!(entry.depend1 == -1 && entry.depend2 == -1))
    return;

  if (++entry.clock < 3)
    return;
  pop();

  unsigned ans = 0;
  int dest = entry.dest;
  unsigned v1 = entry.value1, imm = RoB[dest].imm;
  InstructionType type = RoB[dest].type;
  imm = signExtend<12>(imm);
  ans = v1 + imm; //!!! store
  switch (type) {
  case LB:
    ans = mem.readSigned<1>(ans);
    break;
  case LH:
    ans = mem.readSigned<2>(ans);
    break;
  case LW:
    ans = mem.read<4>(ans);
    break;
  case LBU:
    ans = mem.read<1>(ans);
    break;
  case LHU:
    ans = mem.read<2>(ans);
    break;
  default:
    RoB[dest].value2 = entry.value2;
    stall_ = 1;
    break;
  }

  RoB[dest].value = ans;
  RoB[dest].ready = 1;
  if (!isSType(type))
    RoB.broadcast(RS, *this, dest);
}
