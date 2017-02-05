#include <iostream>
#include <vector>
using namespace std;

int main() {

  //basic example
  int a = 3, b = 5;
  auto sum = a + b;
  cout << sum <<endl;

  //vector example
  vector<int> container{1,2,3,4,5};
  vector<int>::size_type sz = container.size();
  auto sz2 = container.size();
  cout<< sz << " " << sz2 <<endl;

  for(const auto &i : container) {
    cout<<i<<"\t";
  }
  cout<<endl;

  const char name[] = "Jimin Jeon"; // const char[11]
  auto aName = name; // const char *
  auto & rName = name; //const char (&)[11]
  cout<<aName<<endl;
  cout<<rName<<endl;
}
