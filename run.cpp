#include <bits/stdc++.h>
using namespace std;

string test_case[18];

int main() {
  freopen("results.txt", "w", stdout);
  test_case[0] = "array_test1";
  test_case[1] = "array_test2";
  test_case[2] = "basicopt1";
  test_case[3] = "bulgarian";
  test_case[4] = "expr";
  test_case[5] = "gcd";
  test_case[6] = "hanoi";
  test_case[7] = "lvalue2";
  test_case[8] = "magic";
  test_case[9] = "manyarguments";
  test_case[10] = "multiarray";
  test_case[11] = "naive";
  test_case[12] = "pi";
  test_case[13] = "qsort";
  test_case[14] = "queens";
  test_case[15] = "statement_test";
  test_case[16] = "superloop";
  test_case[17] = "tak";
  for (int i = 0; i < 18; i++) {
    char s[100];
    sprintf(s, "./code < ./testcases/%s.data", test_case[i].data());
    cout << test_case[i] << (test_case[i].size() < 8 ? "\t" : "") << "\t\t"
         << flush;
    system(s);
  }
  return 0;
}
