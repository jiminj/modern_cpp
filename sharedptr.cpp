#include <iostream>
#include <memory>

class Integer
{
  int n;
  public:
    Integer(int n): n{n} { std::cout<< "Ctor called -- " << n << std::endl; }
    ~Integer() { std::cout << "Dtor called --" << n << std::endl; }
    int get() const { return n; }
};
void func(const std::shared_ptr<Integer> & ptr) {
  std::cout<<ptr->get()<<std::endl;
}

struct Cyclic {
  std::weak_ptr<Cyclic> ptr;
  Cyclic() { std::cout << "Cyclic - ctor called"<<std::endl; }
  ~Cyclic() { std::cout << "Cyclic - dtor called"<<std::endl; }
};

int main() {
  std::shared_ptr<Integer> a{new Integer{10}};
  func(a);
  std::cout<< a->get() <<std::endl;

  auto x = std::make_shared<Cyclic>();
  auto y = std::make_shared<Cyclic>();
  x->ptr = y;
  y->ptr = x;

  auto shared = std::make_shared<int>(42);
  std::weak_ptr<int> weak = shared;
  auto sw = weak.lock();
  std::cout<<*sw<<std::endl;

  auto pAuto = std::make_unique<int>(5);
}

