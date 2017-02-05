#include <iostream>
using namespace std;

template<class T, class U>
auto add(T t, U u) -> decltype(t + u) // the return type is the type of operator+(T, U)
{
  return t + u;
}

template<class T, class U>
auto add14(T t, U u) {
  return t + u;
}

int main() {
  double a;
  auto i = 5;
  decltype(a) j = i;
  cout<<typeid(j).name()<<":"<< j<<endl; //d:5

  cout<<add(3,5)<<endl;
  cout<<add14(3,5)<<endl;
}

