#ifndef _SJTU_RS_HPP_
#define _SJTU_RS_HPP_

#include "instruction.hpp"
#include "register.hpp"

class ReorderBuffer;
class LoadStoreBuffer;

struct ReservationStationEntry {
public:
  bool busy = 0;
  unsigned value1, value2;
  int depend1, depend2;
  int dest; // RoB index

public:
  ReservationStationEntry() = default;
  ReservationStationEntry(ReorderBuffer &RoB, RegisterFile &reg, unsigned rs1,
                          unsigned rs2, int d);
};

class ReservationStation {
  friend class ReorderBuffer;
  friend class ControlUnit;

public:
  static constexpr int SIZE = 16;

private:
  ReservationStationEntry cur[SIZE], next[SIZE];
  int size = 0, size_next = 0;

public:
  void update(ReorderBuffer &RoB);
  bool empty() const { return !size; }
  bool full() const { return size == SIZE; }
  void clear() {
    size_next = 0;
    for (auto &entry : next)
      entry.busy = 0;
  }
  inline void issue(ReorderBuffer &RoB, RegisterFile &reg,
                    const Instruction &ins, int dest) {
    for (int i = 0; i < SIZE; i++)
      if (!cur[i].busy) {
        next[i] = ReservationStationEntry(RoB, reg, ins.rs1, ins.rs2, dest);
        size_next++;
        return;
      }
  }
  void execute(ReorderBuffer &RoB, LoadStoreBuffer &LSB);
};

#endif