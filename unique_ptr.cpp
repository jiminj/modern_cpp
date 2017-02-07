#include <iostream>
#include <memory>
#include <vector>

int main() {
  std::unique_ptr<int> p1;
  auto p2 = std::make_unique<int>(5);

  p1 = std::move(p2);
  //std::cout<<*p1<< " "<<*p2<<std::endl;
  std::cout<<*p1<<std::endl;
  std::cout<<(p1==nullptr)<<std::endl;

  //std::vector<std::unique_ptr<int>> vec;
  //vec.push_back(std::move(p2));
}
