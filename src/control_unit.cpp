#include "control_unit.hpp"
#include "load_store_buffer.hpp"
#include "predictor.hpp"
#include "reorder_buffer.hpp"
#include "reservation_station.hpp"
#include <algorithm>
#include <cstdlib>

#define DEBUG
#include <iostream>
#include <string>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
#ifdef DEBUG
#include <cassert>
#define SHOW(x) (cerr << #x "=" << (x) << endl, (x))
#else
#define SHOW(x) (x)
#endif

void ControlUnit::fetchInstruction(ReservationStation &RS, LoadStoreBuffer &LSB,
                                   ReorderBuffer &RoB, RegisterFile &reg,
                                   Memory &mem, BranchPredictor &BP) {
  if (RoB.full() || stall_)
    return;
  Instruction ins(mem.read<4>(PC));
  InstructionType type = ins.type;
  // cerr << "fetch: " << instructionName[type] << ' ' << hex << PC
  //      << endl; //!!!!!!!!!!!
  if (isSimple(type)) {
    if (type == NONE) {
      stall_ = 1;
      return;
    } else if (type == END) {
      stall_ = 1;
    }
  } else if (isLoadOrStore(type)) {
    if (LSB.full())
      return;
    LSB.issue(RoB, reg, ins, RoB.getCurrentIndex());
  } else {
    if (RS.full())
      return;
    RS.issue(RoB, reg, ins, RoB.getCurrentIndex());
    if (type == JALR)
      stall_ = 1; //!!!
  }
  // cerr << "issue: " << instructionName[type] << ' ' << hex << PC
  //      << endl; //!!!!!!!!!!!
  bool prediction = isJumpOrBranch(type) ? BP.predict(ins) : 0;
  RoB.issue(reg, ins, PC, prediction);
  if (prediction) {
    PC += ins.imm;
  } else if (!stall_) {
    PC += 4;
  }
}

namespace {
void fetchInstruction(ControlUnit &CU, ReservationStation &RS,
                      LoadStoreBuffer &LSB, ReorderBuffer &RoB,
                      RegisterFile &reg, Memory &mem, BranchPredictor &BP) {
  CU.fetchInstruction(RS, LSB, RoB, reg, mem, BP);
}
void RSExecute(ControlUnit &CU, ReservationStation &RS, LoadStoreBuffer &LSB,
               ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
               BranchPredictor &BP) {
  RS.execute(RoB, LSB);
}
void LSBExecute(ControlUnit &CU, ReservationStation &RS, LoadStoreBuffer &LSB,
                ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
                BranchPredictor &BP) {
  LSB.execute(RoB, RS, mem);
}
void RoBCommit(ControlUnit &CU, ReservationStation &RS, LoadStoreBuffer &LSB,
               ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
               BranchPredictor &BP) {
  RoB.commit(CU, RS, LSB, reg, mem);
}
} // namespace

void (*module[4])(ControlUnit &CU, ReservationStation &RS, LoadStoreBuffer &LSB,
                  ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
                  BranchPredictor &BP) = {fetchInstruction, RSExecute,
                                          LSBExecute, RoBCommit};
int perm[4] = //{3, 2, 1, 0};
    {2, 1, 0, 3};
bool ControlUnit::run(ReservationStation &RS, LoadStoreBuffer &LSB,
                      ReorderBuffer &RoB, RegisterFile &reg, Memory &mem,
                      BranchPredictor &BP) {
  // 5 concurrent steps: instruction fetch, instruction decode, execution,
  // memory access, write back

  // std::random_shuffle(module, module + 3);
  for (int i = 0; i < 4; i++)
    module[perm[i]](*this, RS, LSB, RoB, reg, mem, BP);
  // fetchInstruction(RS, LSB, RoB, reg, mem, BP);
  // RS.execute(RoB, LSB);
  // LSB.execute(RoB, RS, mem);
  // RoB.commit(*this, RS, LSB, reg, mem);
  update(RS, LSB, RoB, reg);
  clk++;
  return !end_;
  // return !(RoB.empty() && stall_); //!
}
