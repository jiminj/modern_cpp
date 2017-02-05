#include <iostream>
#include <vector>
#include <algorithm>
#include <complex>

int main() {

  int one = 1;
  std::function<int(int)> addOne = [one](int n){return one + n;};
  std::cout<<addOne(2)<<std::endl;

  int sum = 0;
  auto add = [&sum](int n){ sum += n; };

  std::vector<int> nums {5, 4, 3, 2, 1};
  std::sort(nums.begin(), nums.end(), [](int a, int b) { return a < b; });
  std::for_each(nums.begin(), nums.end(), [](int &n){ n++; });
  std::for_each(nums.begin(), nums.end(), add);
  for(auto i : nums) {
    std::cout<<i<<" "; 
  }
  std::cout<<std::endl<<"SUM :" << sum<<std::endl;
  /*
  auto square = [](auto n) { return n * n; }; //C++14
  std::cout<< square(2)<<std::endl;
  std::cout<< square(3.14)<<std::endl;
  std::cout<< square(std::complex<double>(3,-2))<<std::endl;
  */

}
