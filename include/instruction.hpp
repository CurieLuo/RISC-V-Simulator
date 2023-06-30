#ifndef _SJTU_INSTRUCTION_HPP_
#define _SJTU_INSTRUCTION_HPP_

// #define DEBUG
#include <iostream>
#include <string>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::hex;
using std::string;

#ifdef DEBUG
#include <cassert>
#define SHOW(x) (cerr << #x "=" << (x) << endl, (x))
#else
#define SHOW(x) (x)
#endif
const string instructionName[] = {
    "NONE", "END",  "LUI",  "AUIPC", "JAL",  "JALR", "BEQ",   "BNE",
    "BLT",  "BGE",  "BLTU", "BGEU",  "LB",   "LH",   "LW",    "LBU",
    "LHU",  "SB",   "SH",   "SW",    "ADDI", "SLTI", "SLTIU", "XORI",
    "ORI",  "ANDI", "SLLI", "SRLI",  "SRAI", "ADD",  "SUB",   "SLL",
    "SLT",  "SLTU", "XOR",  "SRL",   "SRA",  "OR",   "AND"}; // for debug

enum InstructionType {
  NONE,  // invalid instruction (caused by wrong prediction)
  END,   // End simulator program & output reg[10] modulo 256
  LUI,   // Load Upper Immediate
  AUIPC, // Add Upper Immediate to PC
  JAL,   // Jump and Link
  JALR,  // Jump and Link Register
  BEQ,   // Branch if Equal
  BNE,   // Branch if Not Equal
  BLT,   // Branch if Less Than
  BGE,   // Branch if Greater Than or Equal
  BLTU,  // Branch if Less Than, Unsigned
  BGEU,  // Branch if Greater Than or Equal, Unsigned
  LB,    // Load Byte
  LH,    // Load Half Word
  LW,    // Load Word
  LBU,   // Load Byte, Unsigned
  LHU,   // Load Half Word, Unsigned
  SB,    // Store Byte
  SH,    // Store Half Word
  SW,    // Store Word
  ADDI,  // Add Immediate
  SLTI,  // Set if Less Than Immediate
  SLTIU, // Set if Less Than Immediate, Unsigned
  XORI,  // Exclusive OR Immediate
  ORI,   // OR Immediate
  ANDI,  // AND Immediate
  SLLI,  // Shift Left Immediate
  SRLI,  // Shift Right Logical Immediate
  SRAI,  // Shift Right Arithmetic Immediate
  ADD,   // Add
  SUB,   // Subtract
  SLL,   // Shift Left
  SLT,   // Set if Less Than
  SLTU,  // Set if Less Than, Unsigned
  XOR,   // Exclusive OR
  SRL,   // Shift Right Logical
  SRA,   // Shift Right Arithmetic
  OR,    // OR
  AND    // And
};

inline bool isJumpOrBranch(InstructionType type) {
  return JAL <= type && type <= JALR || BEQ <= type && type <= BGEU;
}
inline bool isLoadOrStore(InstructionType type) {
  return LB <= type && type <= SW;
}
inline bool isSimple(InstructionType type) { // no register/memory input
  return type <= JAL;
}

inline bool isBType(InstructionType type) {
  return BEQ <= type && type <= BGEU;
}
inline bool isIType(InstructionType type) {
  return type == JALR || LB <= type && type <= LHU ||
         ADDI <= type && type <= SRAI;
}
inline bool isSType(InstructionType type) { return SB <= type && type <= SW; }

template <int digit> inline signed signExtend(unsigned x) {
  return (x >> (digit - 1) & 1) ? (x | ~0u >> digit << digit) : x;
}
template <int U, int L> inline unsigned getRange(unsigned code) {
  return code >> L & ((1 << (U - L)) - 1);
}

struct Instruction {
public:
  InstructionType type;
  unsigned rd, rs1, rs2;
  unsigned imm;

public:
  Instruction() = default;
  Instruction(unsigned code);
};

#endif