#include <iostream>

template <int N> struct FactorialTM { enum { value = FactorialTM<N-1>::value * N }; };
template <> struct FactorialTM<0> { enum { value = 1 }; };

constexpr int factorial(int n) {
  return n > 0 ? n * factorial(n-1) : 1;
}

int main() {
  std::cout<<FactorialTM<5>::value<<std::endl;
  std::cout<<factorial(5)<<std::endl;
}
