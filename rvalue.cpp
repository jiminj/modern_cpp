#include <iostream>
#include <string>

void printString(const std::string & str) {
  std::cout<<str<<std::endl;
}
void printString(std::string && str) {
  std::cout<<str<<std::endl;
}

int main() {
  const std::string & lvalRef = std::string("hello");
  std::string && rvalRef = std::string("world");
  std::cout<<lvalRef<< " "<<rvalRef<<std::endl;
  std::string name{"hi!"};
  printString("hello");
  printString(name);
}

