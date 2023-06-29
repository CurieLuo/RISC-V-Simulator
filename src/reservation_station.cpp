#include "reservation_station.hpp"
#include "control_unit.hpp"
#include "load_store_buffer.hpp"
#include "reorder_buffer.hpp"

ReservationStationEntry::ReservationStationEntry(ReorderBuffer &RoB,
                                                 RegisterFile &reg,
                                                 unsigned rs1, unsigned rs2,
                                                 int d)
    : busy(1), dest(d), value1(0), value2(0) {
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

void ReservationStation::receive(int dest, unsigned value) {

  for (auto &entry : table)
    if (entry.busy) {
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

void ReservationStation::execute(ReorderBuffer &RoB, LoadStoreBuffer &LSB) {
  for (auto &entry : table) {
    if (!(entry.busy && entry.depend1 == -1 && entry.depend2 == -1))
      continue;
    entry.busy = 0;
    size--;
    // TODO ALU
    unsigned ans = 0;
    int dest = entry.dest;
    unsigned v1 = entry.value1, v2 = entry.value2, imm = RoB[dest].imm;
    InstructionType type = RoB[dest].type;
    switch (type) {
    case JALR:
      RoB[dest].value2 = (v1 + imm) & ~1u; //!!!
      break;

    case BEQ:
      ans = v1 == v2;
      break;
    case BNE:
      ans = v1 != v2;
      break;
    case BLT:
      ans = (int)v1 < (int)v2;
      break;
    case BGE:
      ans = (int)v1 >= (int)v2;
      break;
    case BLTU:
      ans = v1 < v2;
      break;
    case BGEU:
      ans = v1 >= v2;
      break;

    case SLTI:
      ans = (int)v1 < (int)imm;
      break;
    case SLTIU:
      ans = v1 < imm;
      break;
    case SLT:
      ans = (int)v1 < (int)v2;
      break;
    case SLTU:
      ans = v1 < v2;
      break;

    case SLLI:
      ans = v1 << imm;
      break;
    case SRLI:
      ans = v1 >> imm;
      break;
    case SRAI:
      ans = (int)v1 >> (int)imm;
      break;
    case SLL:
      ans = v1 << v2;
      break;
    case SRL:
      ans = v1 >> v2;
      break;
    case SRA:
      ans = (int)v1 >> (int)v2;
      break;

    case XORI:
      ans = v1 ^ imm;
      break;
    case ORI:
      ans = v1 | imm;
      break;
    case ANDI:
      ans = v1 & imm;
      break;
    case XOR:
      ans = v1 ^ v2;
      break;
    case OR:
      ans = v1 | v2;
      break;
    case AND:
      ans = v1 & v2;
      break;

    case ADDI:
      ans = v1 + imm;
      break;
    case ADD:
      ans = v1 + v2;
      break;
    case SUB:
      ans = v1 - v2;
      break;
    default:
      break;
    }
    if (type != JALR)
      RoB[dest].value = ans;
    RoB[dest].ready = 1;
    if (!isBType(type)) {
      RoB.broadcast(*this, LSB, dest);
    }
    return;
  }
}
