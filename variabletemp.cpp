#include <iostream>

template<typename T>
  constexpr T PI = T(3.1415926535897932385);

template<typename T>
  T area (T r) { return PI<T> * r * r; }

int main() {
  std::cout << PI<int> << " " << area(2) << std::endl;
  std::cout<< PI<double> << " "<<  area(2.0) << std::endl;
  //output
  //3 12
  //3.14159 12.5664
}
