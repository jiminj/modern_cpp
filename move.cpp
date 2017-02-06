#include <iostream>
#include <string>

struct Obj
{
  std::string s;
  Obj() : s{"text"} {}
  Obj(const Obj& o) : s{o.s} { std::cout << "copy ctor!"<<std::endl; }
  Obj(Obj&& o) noexcept : s{std::move(o.s)} { std::cout << "move ctor!"<<std::endl; }
  //~Obj() {std::cout<<"dtor called"<<std::endl;}
};

Obj f(Obj o) { return o; } //produce rvalue

Obj someFunc(Obj && o) {
  Obj temp{std::move(o)};
  return temp;
}

int main() {
  Obj o;
  someFunc(Obj());
  //Obj a{o}; //copy ctor!
  //Obj b{f(Obj())}; //move ctor!
  //Obj c{std::move(o)}; //move ctor!
}


