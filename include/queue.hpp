#ifndef _SJTU_QUEUE_HPP_
#define _SJTU_QUEUE_HPP_

template <class T, int N_> struct Queue {
public:
  static constexpr int N = N_ + 1;
  T a[N];

public:
  int head, tail;

public:
  static constexpr int CAP = N_;
  Queue() : head(0), tail(0) {}
  bool empty() const { return tail == head; }
  bool full() const { return (tail + 1) % N == head; }
  const T &front() const { return a[head]; }
  T &front() { return a[head]; }
  void push(const T &x) {
    a[tail] = x;
    tail = (tail + 1) % N;
  }
  void pop() { head = (head + 1) % N; }
  void clear() { head = tail = 0; }
  T &operator[](int idx) { return a[idx]; }
  const T &operator[](int idx) const { return a[idx]; }
};

#endif