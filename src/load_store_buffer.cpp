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

void LoadStoreBuffer::update(ReorderBuffer &RoB) {
  for (int i = next.head; i != next.tail; i = (i + 1) % next.N) {
    LoadStoreBufferEntry &entry = next[i];
    if (entry.depend1 != -1 && RoB[entry.depend1].ready)
      entry.value1 = RoB[entry.depend1].value, entry.depend1 = -1;
    if (entry.depend2 != -1 && RoB[entry.depend2].ready)
      entry.value2 = RoB[entry.depend2].value, entry.depend2 = -1;
  }
  cur = next;
}

void LoadStoreBuffer::execute(ReorderBuffer &RoB, ReservationStation &RS,
                              Memory &mem) {
  if (empty() || stall_)
    return;
  const LoadStoreBufferEntry &entry = cur.front();
  if (!(entry.depend1 == -1 && entry.depend2 == -1))
    return;
  if (++next.front().clock < 3)
    return;
  next.pop();

  unsigned ans = 0;
  int dest = entry.dest;
  unsigned v1 = entry.value1, imm = RoB[dest].imm;
  InstructionType type = RoB[dest].type;
  imm = signExtend<12>(imm);
  ans = v1 + imm;
  switch (type) {
  case LB:
    ans = mem.readSigned<1>(ans);
    break;
  case LH:
    ans = mem.readSigned<2>(ans);
    break;
  case LW:
    ans = mem.readSigned<4>(ans);
    break;
  case LBU:
    ans = mem.read<1>(ans);
    break;
  case LHU:
    ans = mem.read<2>(ans);
    break;
  default:
    RoB.next[dest].value2 = entry.value2;
    stall_ = 1;
    break;
  }
  RoB.next[dest].value = ans;
  RoB.next[dest].ready = 1;
}