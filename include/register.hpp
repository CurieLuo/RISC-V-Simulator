#ifndef _SJTU_REGISTER_HPP_
#define _SJTU_REGISTER_HPP_

struct RegisterFile {
private:
  static const int REG_NUM = 32;

public:
  unsigned cur[REG_NUM], next[REG_NUM]; // register value
  int depend[REG_NUM];                  // RoB index (-1: none)
public:
  void clearDependence() {
    for (int i = 0; i < REG_NUM; i++)
      depend[i] = -1;
  }
  RegisterFile() {
    cur[0] = next[0] = 0;
    clearDependence();
  }
  void update() {
    cur[0] = next[0] = 0, depend[0] = -1;
    for (int i = 1; i < REG_NUM; i++)
      cur[i] = next[i];
  }
};

#endif