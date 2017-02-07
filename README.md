# Modern C++; Changes and New Features Basics
This article is written as a quick reference material for the internal company seminar, aiming to introduce very basic concepts of C++11/14 in approx ~1h of time. Many parts of detailed internal mechanisms or advanced usages are omitted intentionally.

## Type Deduction
### auto
기본적으로 template type deduction과 유사한 법칙에 기반해 동작한다. (자세한 사항은 생략) 

```cpp
//basic example
int a = 3, b = 5;
auto sum = a + b; //deducted to int
```

```cpp
vector<int> container{1,2,3,4,5}; //Uniform initialization
...
vector<int>::size_type sz = container.size();
auto sz2 = container.size(); //동일한 결과

//Range-based loop (C++11 feature)
for(auto i : container) {
  cout<<i<<" "; 
} // expected output: 1 2 3 4 5
```

이런 식으로 타입을 좀 더 구체적으로 지정해 줄 수도 있다.
```cpp
for(const auto i : container) { .. }
for(const auto &i : container) { .. }
```

항상 초기 값이 필요하다.
```cpp
auto a; //error
```
C++11까지는 auto를 return type에는 사용할 수 없다.  C++14에서는가능하다.  
```cpp
template<class T, class U>
auto add14(T t, U u) { //C++14
  return t + u;
}
```

### decltype
주어진 변수, 표현식 등에서 type을 유추한다.
```cpp
//basic example
double a;
auto i = 5;
decltype(a) j = i;
cout<<typeid(j).name()<<":"<< j<<endl; //d:5
```

C++11에서도 함수의 반환형을 추론할 수 있다.
```cpp
template<class T, class U>
auto add(T t, U u) -> decltype(t + u) { //trailing return type
  return t + u;
}
```

*더 읽을거리*: `decltype(auto)`

## Lambda Expression

![](http://www.codeguru.com/images/article/16693/lambdaparts.png)

(이미지 출처: codeguru.com)

기본적인 rules :
- [] : capture
- () : parameters
- {} : body

return type은 생략될 때가 많다. (이 때, return type은 auto deduction rule을 따른다.)

```cpp
  int one = 1;
  auto addOne = [one](int n){return one + n;};
  //이 때 addOne의 타입을 std::function<int(int)> 으로 지정할 수도 있으나, 둘이 완전히 동일한 것은 아니다.
  std::cout<<addOne(2)<<std::endl;
```

Capture rules는 아래와 같다.

- **[a,&b]** a는 value로, b는 reference로
- **[this]** this 포인터
- **[&]** 접근 가능한 모든 변수를 references로
- **[=]** 접근 가능한 모든 변수를 value로
- **[]**  캡춰하지 않는다. 


사용 예시

```cpp
  int sum = 0;
  auto add = [&sum](int n){ sum += n; };

  std::vector<int> nums {5, 4, 3, 2, 1};
  std::sort(nums.begin(), nums.end(), [](int a, int b) { return a < b; });
  std::for_each(nums.begin(), nums.end(), [](int &n){ n++; });
  std::for_each(nums.begin(), nums.end(), add); //same as std::accumulate

  for(auto i : nums) { cout<<i<<" "; }
  cout<<endl<<"SUM :" << sum<<endl;
```

C++14부터는 capture 할 변수의 이름을 지정할 수 있다. (Initialization capture)

```cpp
  int x = 1;
  auto addOne = [&z = x]() { z += 1; };
  addOne();
  std::cout << x << std::endl;
```

### Generic Lambda
C++14부터 지원한다. parameters를 auto로 받을 수 있다.
```cpp
  auto square = [](auto n) { return n * n; }; //C++14
  std::cout << square(2) << std::endl; //4
  std::cout << square(3.14) << std::endl; //9.8596
  std::cout << square(std::complex<double>(3,-2)) << std::endl; //(5, -12)
```

## Move Semantics

### Rvalue reference
**lvalue expressions** and **rvalue expressions**: *An **lvalue** refers to an object that persists beyond a single expression. You can think of **an lvalue as an object that has a name**. All variables, including nonmodifiable ( const ) variables, are lvalues. An **rvalue** is **a temporary value that does not persist beyond the expression** that uses it.* (ref: MSDN)

```cpp
std::string getName();
int main() {
  int x = 3;  //int x는 lvalue
  getName()   //getName()은 rvalue
}
```

C++11 이전까지는 rvalue에 대한 reference는 const로만 지정할 수 있었다.
```cpp
std::string &v = std::string("hello"); //error!!
const std::string &v = std::string("world"); //okay

//function parameter의 예
void printStringRef(std::string &str);
void printStringConstRef(const std::string &str);
...
printStringRef("hello"); //error
printStringConstRef("world"); //okay
```

C++11에서 도입된 rvalue reference (&&)를 이용하면 rvalue에 대한 reference를 만들 수 있다. 
```cpp
std::string getName();
...
std::string && ref = getName();
std::string && ref2 = std::string("hello!");

//function parameter의 예. 
void printString(std::string && str); //rvalue parameter
void printString(const std::string &str); //lvalue parameter
...
printString("hello"); //rvalue version이 호출
printString(str); //lvalue version이 호출
```

rvalue reference를 반환하는 함수와 같은 expressions을 *expiring value;xvalue* 라고 부른다. 이는 lvalue의 일종으로 취급되며 (generalized lvalue) 주소를 참조할 수 있다.

```cpp
int && xvalue(); 

xvalue(); //xvalue
std::move(instance); //xvalue, 아래 설명
```

![](https://i.stack.imgur.com/GNhBF.png)

**rvalue reference는 type이고, lvalue, xvalue, rvalue..등은 expression임을 유의한다.**

### Move Constructor
복사 생성자(Copy constructor) 대신, 임시 값인 rvalue를 이용하여 객체 생성을 빠르게 할 수 있다. move constructor는 인자가 rvalue일 때 (prvalue이거나, xvalue일 때) 호출된다.

```cpp
struct Obj
{
  std::string s;
  Obj() : s{"text"} {}
  Obj(const Obj& o) : s{o.s} { std::cout << "copy ctor!"<<std::endl; }
  Obj(Obj&& o) noexcept : s{std::move(o.s)} { std::cout << "move ctor!"<<std::endl; }
};

Obj f(Obj o) { return o; } //produce rvalue

int main() {
  Obj o;
  Obj a{o}; //copy ctor!
  Obj b{f(Obj())}; //move ctor!
}
```

### std::move
`std::move`는 기본적으로 parameter로 주어진 lvalue expression을 rvalue reference로 `static_cast` 하는 역할을 한다. (xvalue expression) 이를 이용해 명시적으로 move constructor를 호출할 수 있다. 

```cpp
Obj someFunc(Obj && o) {
  Obj temp1{o}; //move ctor!
  Obj temp2{std::move(o)}; //casting to rvalue(xvalue) - move ctor!
  return temp2;
}
```

**더 읽을거리: Universal reference, `std::forward`, Rule of five**

## Smart Pointers
편리한 자원관리를 위해 도입되었다. 자원을 객체에 넣고 자원 관리(해제)를 객체에게 위임하는 것이 스마트 포인터의 기본적인 개념이다. 

### std::auto_ptr (C++98/deprecated)
가장 기본적인 스마트 포인터이자 C++11 이전까지는 유일하게 지원되는 스마트 포인터였다. 객체가 소멸될 때 (destructor가 호출될 때) 자원이 해제된다. 즉, 자원에 대한 Ownership을 가진다. (Resource Acquisition is Initilization; **RAII Principle**)


일반적인 자원 할당/소멸의 과정은 다음과 같다. 
```cpp
class Widget(){ ... };
void f() {
  Widget * w = new Widget(); 
  ...
  delete w;
}
```
그러나 위 예시는 실행 도중 (`...` 부분) early return, exception 등의 원인으로 자원의 해제(`delete`)가 실행되지 않을 가능성을 가지고 있다. 스마트 포인터를 사용하면 이러한 위험을 피할 수 있다. 
```cpp
class Widget(){ ... };
void f() {
  std::auto_ptr<Widget> w(new Widget()); 
  ...
  //stack이 소멸되며 w의 소멸자가 호출되고, 할당한 자원 (new Widget())이 해제된다.
}
```


그러나 `auto_ptr`은 아래와 같은 이유 때문에 deprecated 되었다.
- `auto_ptr`은 자신이 소멸될 때 자원이 해제되므로, 복사가 가능해서는 안된다. (자원에 대한 해제가 여러번 일어날 수 있다.) 즉, 언제나 unique해야 한다. **이러한 강력한 ownership을 유지하기 위하여 복사가 이루어지면 원본의 객체를 `nullptr`로 만들어버린다.**
```cpp
  std::auto_ptr<Widget> pw1(new Widget());
  std::auto_ptr<Widget> pw2(pw1); //pw1은 nullptr이 된다.
  pw1->someFunc(); //segfault
```

- 위와 같이 copy semantics가 다른 STL containers와는 다르기 때문에 다른 STL containers (`std::vector`, `std::map`..)의 원소로 사용될 수 없다.
- 자원의 해제에 `delete[]`가 아닌 `delete`를 이용하므로 배열(plain array)을 자원으로 가질 수 없다.

### std::shared_ptr
가장 보편적으로 사용되는 smart pointer이다. reference counting을 이용해 수명 주기를 관리한다. 참조하고 있는 외부 객체의 수(참조 횟수)가 0이 되면 자동으로 해제된다. `auto_ptr`과 마찬가지의 이유로 배열을 담을 수 없다.

```cpp
std::shared_ptr<Widget> spw1(new Widget); 
//std::make_shared를 이용하면 코드도 간결해지며 성능상의 이점도 있다.
auto spw2(std::make_shared<Widget>()); 
```
```cpp
class Integer {
  int n;
  public:
    Integer(int n): n{n} { std::cout<< "ctor called -- " << n << std::endl; }
    ~Integer() { std::cout << "dtor called -- " << n << std::endl; }
    int get() const { return n; }
};

int main() {
  std::shared_ptr<Integer> a{new Integer{10}};
  std::shared_ptr<Integer> b{a};
  std::cout<< a->get() <<std::endl;
  std::cout<< b->get() <<std::endl;
}
//출력:
//ctor called -- 10
//10
//10
//dtor called -- 10
```

### std::weak_ptr
Reference counting 방식의 smart pointer의 문제점은 바로 cyclic reference가 발생할 가능성이 있다는 것이다. 

```cpp
struct Widget { std::shared_ptr<Widget> ptr; };
int main()
{
  auto x = std::make_shared<Widget>();
  auto y = std::make_shared<Widget>();

  x->ptr = y; 
  y->ptr = x; // x keeps y alive and y keeps x alive
}
```

`std::weak_ptr`은 `std::shared_ptr` 또는 같은 `std::weak_ptr`로부터만 생성이 가능하며, `std::shared_ptr`의 reference count를 증가시키지 않음으로써 잠재적인 위험에서 벗어날 수 있다. 객체의 수명주기에 영향을 미치지 않으면서 값을 참조하고 싶을 때 사용될 수 있다.

```cpp
struct Widget { std::weak_ptr<Widget> ptr; };
int main()
{
  auto x = std::make_shared<Widget>();
  auto y = std::make_shared<Widget>();

  x->ptr = y; 
  y->ptr = x; 
}
```
`std::shared_ptr` 과는 달리 내부 자원에 바로 접근할 수 없다. 대신  `std::shared_ptr`를 반환하는 `std::weak_ptr::lock()`함수를 제공한다. 

```cpp
  auto shared = std::make_shared<int>(42);
  std::weak_ptr<int> weak = shared;
  std::cout << *weak << std::endl; //error; *weak으로 접근할 수 없다. 
  auto sw = weak.lock();
  std::cout << *sw << std::endl; //output: 42
```

### std::unique_ptr (boost::scoped_ptr)

`std::auto_ptr`의 대체재로 도입되었다. 복사를 통한 생성(copy constructor)은 지원하지 않으며, 대신 `std::move`를 이용하여 ownership trasfer를 통한 생성(move constructor / move assignment operator)을 허용한다. 

```cpp
std::unique_ptr<int> p1(new int(5));
//shared_ptr과 마찬가지로 make_unique를 통한 생성을 허용한다. (C++14)
auto pAuto = std::make_unique<int>(5);

std::unique_ptr<int> p2 = p1; //error!!
std::unique_ptr<int> p3 = std::move(p1); //okay.  p1은 소멸된다.
```

다른 containers에도 `std::move`를 이용해 담을 수 있다.
```cpp
auto ptr = std::make_unique<int>(5);
std::vector<std::unique_ptr<int>> vec;
vec.push_back(ptr); //error
vec.push_back(std::move(ptr)); //okay
```

또한, 내부적으로 자원 해제시에 `delete[]`를 이용하므로 배열을 다룰 수 있다는 차이도 존재한다.


## Compile-time Validation

## Concurrency
### std::async
### std::lock_guard

## STL Containers


