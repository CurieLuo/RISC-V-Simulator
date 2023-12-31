#include "branch_predictor.hpp"
#include "control_unit.hpp"
#include "load_store_buffer.hpp"
#include "reorder_buffer.hpp"
#include "reservation_station.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

void getSimulatorInput(Memory &mem) {
  //! test
  //   std::fstream fin;
  //   string name;
  //   cin >> name;
  //   fin.open(("./testcases/" + name + ".data").data(), std::ios::in);
  // #define cin fin
  //! test
  string s;
  unsigned pos = 0;
  while (cin >> s) {
    if (s[0] == '@')
      pos = std::stoi(s.substr(1), nullptr, 16);
    else
      mem.write<1>(pos++, std::stoi(s, nullptr, 16));
  }
}

signed main() {
  cerr << hex;
  srand(time(0));
  RegisterFile reg;
  Memory mem;
  BranchPredictor BP;
  ReorderBuffer RoB;
  ReservationStation RS;
  LoadStoreBuffer LSB;
  ControlUnit CU;
  getSimulatorInput(mem);
  while (!CU.end())
    CU.run(RS, LSB, RoB, reg, mem, BP);
  cout << (reg.cur[10] & 255u) << endl;
  return 0;
}
