#include <iostream>
#include <memory>

int main() {
  std::auto_ptr<int> p1;
  std::auto_ptr<int> p2(new int(5));
  p1 = p2;
  std::cout<<*p1<<std::endl;
}
