#include <string>
#include <iostream>
#include <complex>

template<typename T>
T adder(T v) { return v; }

template<typename T, typename... Args>
T adder(T first, Args... args) {
  return first + adder(args...);
}

int main() {
  long sum = adder(1, 2, 3, 8, 7);

  std::string s1 = "x", s2 = "aa", s3 = "bb", s4 = "yy";
  std::string ssum = adder(s1, s2, s3, s4);

  float t1 = 1.5;
  int t2 = 1;
  std::complex<double> sum_difftype = adder(t1, t2);

  std::cout<<sum<< " "<< ssum<<" " << sum_difftype<<std::endl;
  //output
  //21 xaabbyy (2.5,0)
}
