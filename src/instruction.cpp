#include "instruction.hpp"
namespace {

inline unsigned getOpCode(unsigned code) { return getRange<7, 0>(code); }
inline unsigned getRd(unsigned code) { return getRange<12, 7>(code); }
inline unsigned getSubtype(unsigned code) { return getRange<15, 12>(code); }
inline unsigned getRs1(unsigned code) { return getRange<20, 15>(code); }
inline unsigned getRs2(unsigned code) { return getRange<25, 20>(code); }
inline unsigned getLowerImm(unsigned code) { return getRange<32, 20>(code); }
inline unsigned getUpperImm(unsigned code) {
  return getRange<32, 12>(code) << 12;
}
// inline unsigned getUpperImm(unsigned code) {return code &0xFFFFF000;}
inline unsigned getJALImm(unsigned code) {
  return getRange<32, 31>(code) << 20 | getRange<31, 21>(code) << 1 |
         getRange<21, 20>(code) << 11 | getRange<20, 12>(code) << 12;
}
inline unsigned getBranchImm(unsigned code) {
  return getRange<32, 31>(code) << 12 | getRange<31, 25>(code) << 5 |
         getRange<12, 8>(code) << 1 | getRange<8, 7>(code) << 11;
}
inline unsigned getStoreImm(unsigned code) {
  return getRange<32, 25>(code) << 5 | getRange<12, 7>(code);
}
} // namespace

Instruction::Instruction(unsigned code) {
  if (code == 0x0FF00513) {
    type = END;
    rd = rs1 = rs2 = imm = 0;
    return;
  }
  unsigned subtype = getSubtype(code), opSign = getRange<31, 30>(code);
  rd = getRd(code), rs1 = getRs1(code), rs2 = 0;
  switch (getOpCode(code)) {
  case 0b0110111:
    type = LUI;
    imm = getUpperImm(code);
    rs1 = 0;
    break;
  case 0b0010111:
    type = AUIPC;
    imm = getUpperImm(code);
    rs1 = 0;
    break;
  case 0b1101111:
    type = JAL;
    imm = getJALImm(code);
    rs1 = 0;
    break;
  case 0b1100111:
    type = JALR;
    imm = getLowerImm(code);
    break;
  case 0b1100011:
    imm = getBranchImm(code);
    rs2 = getRs2(code);
    rd = 0;
    switch (subtype) {
    case 0b000:
    case 0b001:
      type = InstructionType(BEQ + (subtype - 0b000));
      break;
    case 0b100:
    case 0b101:
      type = InstructionType(BLT + (subtype - 0b100));
      break;
    case 0b110:
    case 0b111:
      type = InstructionType(BLTU + (subtype - 0b110));
      break;
    }
    break;
  case 0b0000011:
    imm = getLowerImm(code);
    switch (subtype) {
    case 0b000:
    case 0b001:
    case 0b010:
      type = InstructionType(LB + (subtype - 0b000));
      break;
    case 0b100:
    case 0b101:
      type = InstructionType(LBU + (subtype - 0b100));
      break;
    }
    break;
  case 0b0100011:
    imm = getStoreImm(code);
    rs2 = getRs2(code);
    rd = 0;
    type = InstructionType(SB + (subtype - 0b000));
    break;
  case 0b0010011:
    imm = getLowerImm(code);
    switch (subtype) {
    case 0b000:
      type = ADDI;
      break;
    case 0b010:
    case 0b011:
      type = InstructionType(SLTI + (subtype - 0b010));
      break;
    case 0b100:
      type = XORI;
      break;
    case 0b110:
    case 0b111:
      type = InstructionType(ORI + (subtype - 0b110));
      break;

    case 0b001:
      type = SLLI;
      imm = getRs2(code); // shamt
      break;
    case 0b101:
      type = InstructionType(SRLI + opSign);
      imm = getRs2(code); // shamt
      break;
    }
    break;
  case 0b0110011:
    rs2 = getRs2(code);
    switch (subtype) {
    case 0b000:
      type = InstructionType(ADD + opSign);
      break;
    case 0b001:
    case 0b010:
    case 0b011:
      type = InstructionType(SLL + (subtype - 0b001));
      break;
    case 0b100:
      type = XOR;
      break;
    case 0b101:
      type = InstructionType(SRL + opSign);
      break;
    case 0b110:
    case 0b111:
      type = InstructionType(OR + (subtype - 0b110));
      break;
    }
    break;
  default:
    type = NONE;
    break;
  }
  if (isBType(type))
    imm = signExtend<13>(imm);
  else if (isIType(type) || isSType(type))
    imm = signExtend<12>(imm);
  else if (type == JAL)
    imm = signExtend<21>(imm);
}