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

struct ReservationStation {
private:
  static constexpr int SIZE = 16;
  ReservationStationEntry table[SIZE];
  int size = 0;

public:
  inline bool empty() const { return !size; }
  inline bool full() const { return size == SIZE; }
  inline void clear() {
    size = 0;
    for (auto &entry : table)
      entry.busy = 0;
  }
  inline void issue(ReorderBuffer &RoB, RegisterFile &reg,
                    const Instruction &ins, int dest) {
    for (auto &entry : table)
      if (!entry.busy) {
        entry = ReservationStationEntry(RoB, reg, ins.rs1, ins.rs2, dest);
        size++;
        return;
      }
  }
  void receive(int dest, unsigned value);
  void execute(ReorderBuffer &RoB, LoadStoreBuffer &LSB);
};

#endif