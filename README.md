# Modern C++; Changes and New Features Basics
This article aims to introduce very basic concepts of C++11/14 to people who are already familiar with C++. Many parts of detailed internal mechanisms or advanced usages are omitted intentionally. You may find more information on the Internet.

The most of content and sample codes referred to various online material such as www.cplusplus.com, www.cppreference.com, [MSDN](https://msdn.microsoft.com/), [StackOverflow](http://stackoverflow.com/) and the book ['Effective Modern C++'](http://shop.oreilly.com/product/0636920033707.do), by Scott Meyers

## Type Deduction
### `auto`
기본적으로 template type deduction과 유사한 법칙에 기반해 동작한다. (자세한 사항은 생략) 

```cpp
//basic example
int a = 3, b = 5;
auto sum = a + b; //deducted to int
```

```cpp
vector<int> container{1,2,3,4,5}; //Uniform initialization
/*...*/
vector<int>::size_type sz = container.size();
auto sz2 = container.size(); //동일한 결과

//Range-based loop (C++11 feature)
for(auto i : container) {
  cout<<i<<" "; 
} // expected output: 1 2 3 4 5
```

이런 식으로 타입을 좀 더 구체적으로 지정해 줄 수도 있다.
```cpp
for(const auto i : container) { /*...*/ } //const int
for(const auto &i : container) { /*...*/ } //const int&
```

항상 초기 값이 필요하다.
```cpp
auto a; //error
```
C++11까지는 auto를 return type에는 사용할 수 없다.  C++14에서는가능하다. (내부적으로 template을 사용한다)
```cpp
template<class T, class U>
auto add14(T t, U u) { //C++14
  return t + u;
}
```

### `decltype`
주어진 변수, 표현식 등에서 type을 유추한다. 이 때의 타입은 코드에서 정의된 그대로, 즉 컴파일러가 알고 있는 타입이다.
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

return type은 생략될 때가 많다. (이 때, return type은 auto deduction rule을 따른다.) parameters 역시 생략 가능하다.

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
C++14부터 지원한다. parameters를 auto로 지정할 수 있다.
```cpp
  auto square = [](auto n) { return n * n; }; //C++14
  std::cout << square(2) << std::endl; //4
  std::cout << square(3.14) << std::endl; //9.8596
  std::cout << square(std::complex<double>(3,-2)) << std::endl; //(5, -12)
```

## Move Semantics

### Rvalue reference
**lvalue expressions** and **rvalue expressions**: _**An lvalue** refers to an object that persists beyond a single expression. You can think of **an lvalue as an object that has a name**. All variables, including nonmodifiable (`const`) variables, are lvalues. An **rvalue** is **a temporary value that does not persist beyond the expression** that uses it._ (ref: MSDN)

```cpp
std::string getName();

int x = 3;  //int x는 lvalue
getName()   //getName()은 rvalue
```

C++11 이전까지는 rvalue에 대한 reference는 const로만 지정할 수 있었다.
```cpp
std::string &v = std::string("hello"); //error!!
const std::string &v = std::string("world"); //okay

//function parameter의 예
void printStringRef(std::string &str);
void printStringConstRef(const std::string &str);
/*...*/
printStringRef("hello"); //error
printStringConstRef("world"); //okay
```

C++11에서 도입된 rvalue reference (&&)를 이용하면 rvalue에 대한 reference를 만들 수 있다. 
```cpp
std::string getName();
/*...*/
std::string && ref = getName();
std::string && ref2 = std::string("hello!");

//function parameter의 예. 
void printString(std::string && str); //rvalue parameter
void printString(const std::string &str); //lvalue parameter
/*...*/
printString("hello"); //rvalue version(std::string &&)이 호출
printString(str); //lvalue version(const std::string &)이 호출
```

rvalue reference를 반환하는 함수와 같은 expressions을 *expiring value;xvalue* 라고 부른다. xvalue는 lvalue의 일종으로 취급되며 (generalized lvalue) 주소를 참조할 수 있다.

```cpp
int && xvalue(); 

xvalue(); //xvalue
std::move(instance); //xvalue, 아래 설명
```

![](https://i.stack.imgur.com/GNhBF.png)

**rvalue reference는 type이고, lvalue, xvalue, rvalue..등은 expression임을 유의한다.**

### Move Constructor
복사 생성자(Copy constructor) 대신, 임시 값인 rvalue를 이용한다. move constructor는 인자가 rvalue일 때 (prvalue이거나, xvalue일 때) 호출된다. 명시적으로 원본 객체의 상태를 보장하지 않음으로써 자원할당을 줄여 성능 향상을 가져올 수 있다.

```cpp
struct Obj
{
  std::string s;
  Obj() : s{"text"} {}
  Obj(const Obj& o) : s{o.s} { std::cout << "copy ctor!"<<std::endl; }
  Obj(Obj&& o) noexcept : s{std::move(o.s)} { std::cout << "move ctor!"<<std::endl; } //예외를 내보내지 않는다.
};

Obj f(Obj o) { return o; } //produce a rvalue

int main() {
  Obj o;
  Obj a{o}; //copy ctor!
  Obj b{f(Obj())}; //move ctor!
}
```

Move constructor를 이용한 성능향상의 가장 큰 예로 `std::vector::resize`를 들 수 있다. C++11 이전까지는 해당 함수가 호출될 경우 컨테이너 내부에 있는 모든 객체를 새로운 메모리 공간에 복사(copy)하는 과정이 이루어졌지만, 이제 객체를 새로운 공간으로 '이동'함으로써 복사로 인한 overhead를 줄어들게 되었다.

그러나, Move constructor는 특성상 exception safety를 보장할 수 없다는 문제가 있다. 즉, 예외가 발생하고 객체 생성이 실패하였을 때 원본 객체의 상태가 온전하다고 장담할 수 없게되며, 이는 위에서 설명한 `std::vector::resize`와 같이 내부적으로 move constructor를 이용하는 연산에서 문제를 일으킬 수 있다. 따라서 기본 move constructor는 예외를 발생시키지 않는 것(`noexcept` 키워드)을 동작 방침으로 하며, 사용자가 지정한 move constructor역시 특수한 경우를 제외하고는 `noexcept`를 명시하는 것이 권장된다. (실제로 STL 컨테이너들이 객체의 이동을 위해 사용하는 함수는 `std::move_if_noexcept`이다. 만약 move constructor가 `noexcept`로 선언되지 않았다면 copy constructor가 호출된다.) 

### `std::move`
`std::move`는 기본적으로 parameter로 주어진 lvalue expression을 rvalue reference로 `static_cast` 하는 역할을 한다. (xvalue expression) 이를 이용해 명시적으로 move constructor를 호출할 수 있다. 

```cpp
Obj someFunc(Obj && o) {
  Obj temp1{o}; //copy ctor!
  Obj temp2{std::move(o)}; //casting to a rvalue(xvalue) - move ctor!
  return temp2;
}
```

**더 읽을거리: Universal reference, `std::forward`, Rule of five**

## Smart Pointers
편리한 자원관리를 위해 도입되었다. 자원을 객체에 넣고 자원 관리(해제)를 객체에게 위임하는 것이 스마트 포인터의 기본적인 개념이다. 

### `std::auto_ptr (C++98/deprecated)`
가장 기본적인 스마트 포인터이자 C++11 이전까지는 유일하게 지원되는 스마트 포인터였다. 객체가 소멸될 때 (destructor가 호출될 때) 자원이 해제된다. 즉, 자원에 대한 Ownership을 가진다. (Resource Acquisition is Initilization; **RAII Principle**)


일반적인 자원 할당/소멸의 과정은 다음과 같다. 
```cpp
class Widget(){ /*...*/ };
void f() {
  Widget * w = new Widget(); 
  ...
  delete w;
}
```
그러나 위 예시는 실행 도중 (`...` 부분) early return, exception 등의 원인으로 자원의 해제(`delete`)가 실행되지 않을 가능성을 가지고 있다. 스마트 포인터를 사용하면 이러한 위험을 피할 수 있다. 
```cpp
class Widget(){ /*...*/ };
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

### `std::shared_ptr`
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

### `std::weak_ptr`
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

### `std::unique_ptr (boost::scoped_ptr)`

`std::auto_ptr`의 대체재로 도입되었다. 복사를 통한 생성(copy constructor)은 지원하지 않으며, 대신 `std::move`를 이용하여 ownership trasfer를 통한 생성(move constructor / move assignment operator)을 허용한다. 

```cpp
std::unique_ptr<int> p1(new int(5));
//shared_ptr과 마찬가지로 make_unique를 통한 생성을 허용한다. (C++14)
auto pAuto = std::make_unique<int>(5);

std::unique_ptr<int> p2 = p1; //error!!
std::unique_ptr<int> p3 = std::move(p1); //okay.  p1의 상태는 보장되지 않는다.
```

다른 containers에도 `std::move`를 이용해 담을 수 있다.
```cpp
auto ptr = std::make_unique<int>(5);
std::vector<std::unique_ptr<int>> vec;
vec.push_back(ptr); //error
vec.push_back(std::move(ptr)); //okay
```

또한, 내부적으로 자원 해제시에 `delete[]`를 이용하므로 배열을 다룰 수 있다는 차이도 존재한다.

## Compile Time Expresssions

### Type Traits
trait은 일반적으로 어떤 타입이 가지고 있는 '특성정보' 를 의미한다. 컴파일 시간에 타입 정보를 확인할 수 있고, 타입 정보를 이용하여 분기를 만들 수 있다. 다른 언어들에서 사용되는 정의와는 정의가 조금 다르다는 것에 유의한다. (e.g., Scala trait, default methods of Java 8) 

```cpp
std::cout << std::boolalpha;
std::cout << std::is_function<int>::value << std::endl; //false
std::cout << std::is_function<int(int)>::value << std::endl; //true

struct A {};
class B : A {};
struct C {};

std::cout << std::is_class<C>::value << std::endl; //true
std::cout << std::is_base_of<A, B>::value << std::endl; //true
std::cout << std::is_base_of<A, C>::value << std::endl; //false
```
그 외에 `std::is_array`, `std::is_pointer`, `std::is_fundamental`, `std::is_void`, `std::is_const`.. 등 다양한 traits들을 제공한다. 


### Static Assertion
컴파일 시간에 주어진 조건을 검사할 수 있다. `static_assert`문을 통한 검사를 통과하지 못하면 컴파일이 실패하게 된다. (runtime 검사를 수행하는 기존 `assert`와는 다르다)

```cpp
template <class T>
void swap(T &a, T &b) {
  static_assert(std::is_copy_constructible<T>::value, "Swap requires copying");
  auto c = b; b = a; a = c;
}
```

### Variadic Template (C++11)
```cpp
template<typename T>
T adder(T v) { return v; }

template<typename T, typename... Args>
T adder(T first, Args... args) { return first + adder(args...); }

int main() {
  long sum = adder(1, 2, 3, 8, 7);

  std::string s1 = "x", s2 = "aa", s3 = "bb", s4 = "yy";
  std::string ssum = adder(s1, s2, s3, s4);

  float t1 = 1.5;
  int t2 = 1;
  std::complex<double> sum_difftype = adder(t1, t2);

  std::cout<<sum<< " "<< ssum<<" " << sum_difftype<<std::endl;
  //output
  //21 xaabbyy (2.5,0)
}
```

### Variable Template (C++14)
```cpp
template<typename T> constexpr T PI = T(3.1415926535897932385);
template<typename T> T area (T r) { return PI<T> * r * r; }

int main() {
  std::cout << PI<int> << " " << area(2) << std::endl;
  std::cout<< PI<double> << " "<<  area(2.0) << std::endl;
  //output
  //3 12
  //3.14159 12.5664
}
```

### constexpr
Generalized constant expression; 특정한 expressions을 컴파일 시간에 평가할 수 있도록 한다.

```cpp
constexpr int multiply(int x, int y) { return x * y; }
const int val = multiply(10, 10); //컴파일 시간에 계산된다.
int my_array[ multiply(2, 10) ];
```

과거 template meta programming과 유사한 측면이 있으나, 조금 더 포괄적으로 사용할 수 있다.

```cpp
constexpr int factorial(int n) {
  return n > 0 ? n * factorial(n-1) : 1; 
}

std::cout<<factorial(5); //120
```
```cpp
//참고: TMP로 구현한 Factorial

template <int N> struct FactorialTM { enum { value = FactorialTM<N-1>::value * N }; };
template <> struct FactorialTM<0> { enum { value = 1 }; };

std::cout << FactorialTM<5>::value; //120
```
다만 컴파일 시간 연산이 언제나 guarantee되는 것은 아니고, 문맥에 따라 runtime에도 연산을 수행할 수 있다.

```cpp
int n;
cin >> n;
factorial(n);
```

변수에도 사용할 수 있다. 의미에 있어 몇 가지 차이가 있지만 기본적으로 `const`와 유사한 역할을 한다. 
```cpp
constexpr auto size = 10;
```
`#define`과 같은 매크로를 대체할 수 있다는 점에서 동작 방식은 다르지만 어느정도 `inline` specifier의 역할을 대체한다고도 할 수 있다.

### Compile Time Static If (C++17) (`if constexpr`)
컴파일 시간에 결정되는 value에 대한 `if` statement 역시 컴파일 시간에 평가될 수 있다.

```cpp
template<typename T>
auto get_value(T t) {
  if constexpr (std::is_pointer_v<T>) { return *t; } //if constexpr (condition)
  else { return t; }
}
```

### Template `auto` (C++17)
```cpp
template <auto n> struct AutoTemp { }
template <auto ... v> struct MixedTypeList {};
using myList = MixedTypeList<10, 'x', 1.3f>;
AutoTemp<5> a;
AutoTemp<'b'> b;
```

### Template Type Deduction (C++17)
Template type argument에 대한 추론이 가능하다.
```cpp
std::pair p(2, 4.5);
std::tuple t(4, 3, 2.5);
```

## Concurrency
### `std::thread` and `std::mutex`
언어 차원에서 동시성 관련 기능을 지원하게 됨으로써 여러 플랫폼에서 표준적인 행동을 보이는 멀티 스레드 프로그램을 작성할 수 있게 되었다. STL에서 제공하는 thread와 mutex의 사용방법은 pthread와 크게 다르지 않다.

```cpp
std::mutex mut;
void work(int n) { 
  mut.lock();
  /* ... */ 
  mut.unlock();
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main(){
  std::thread t1(work, 0);
  std::thread t2([]{ /* lambda thread */ }, 1);
  t1.join();
  t2.join();
}
```

`std::mutex`를 위한 `std::lock_guard` 및 `std::unique_lock` 등의 wrapper를 제공되는데, 이는 mutex의 RAII버전이라고 볼 수 있다. 명시적인 unlock 작업 없이도 해제를 보장한다. (`std::unique_lock`은 객체 생성과 lock을 분리할 수 있다는 차이가 있다.)

```cpp
std::mutex mut;
void work(int n) { 
  std::lock_guard<std::mutex> lock(mut);
  /* ... */
  //unlock 필요 없음
}
```

### `std::async`
비동기 task/job을 수행한다. 동시성이 필요한 작업을 수행할 때, 많은 경우 직접 thread를 생성하는 것 보다는 task기반으로 작업하는 것이 높은 추상성을 유지할 수 있으며, 성능면에서도 유리한 점이 많다. (scheduling, load balancing 등의 복잡한 문제를 직접 처리하지 않아도 된다.)

```cpp
auto func = [] { std::cout << "hello world" << std::endl; };
auto handle = std::async(std::launch::async, func);
```
`std::async`가 결과로 반환하는 것은 `std::future` 객체이다. 이를 통해 task의 결과를 원래의 데이터와 동기화 할 수 있다. 이 때, **결과값을 변수에 저장하지 않는다면** `std::async`가 비동기적으로 수행된다는 보장을 할 수 없음을 유의한다. (바로 `std::future`의 destructor가 호출되며 작업을 block하게 된다) 

```cpp
std::future<std::string> fut = std::async(std::launch::async, [](){ return "The async task is completed"; });
/* .. */
fut.wait(); //block
std::cout<< "result : " << fut.get();
```

정확히 말해 `std::async`는 단순히 주어진 작업을 비동기적으로 수행행한다는 의미 보다는 주어진 정책에 따라 작업을 처리하는 역할을 가진다고 여기는 것이 옳다. `std::async`는 두 개의 인자를 받는데, 첫번 째 인자를 통해 두 번째 인자로 주어진 작업을 바로 (다른 thread를 통해) 비동기 수행할 것인지(`std::launch::async`) 아니면 값이 요구될 때 lazy evaluation할 것인지(`std::launch::deferred`)를 선택할 수 있다. lazy evaluation을 정책으로 결정한 경우, 주어진 작업은 결과로 받은 `std::future`가 데이터가 필요한 시점이 될 때(`get()` 또는 `wait()`이 호출되는 시점) 현재 thread에서 수행된다.

```cpp
void print_ten (char c, int ms) {
  for (int i=0; i<10; ++i) {
    std::this_thread::sleep_for (std::chrono::milliseconds(ms));
    std::cout << c;
  }
}

int main ()
{
  std::cout << "with launch::async:\n";
  std::future<void> foo = std::async (std::launch::async,print_ten,'*',100);
  std::future<void> bar = std::async (std::launch::async,print_ten,'@',200);
  // async "get" (wait for foo and bar to be ready):
  foo.get();
  bar.get();
  std::cout << std::endl;

  //possible output:
  //  with launch::async:
  //  **@**@**@*@**@*@@@@@

  std::cout << "with launch::deferred:\n";
  foo = std::async (std::launch::deferred,print_ten,'*',100);
  bar = std::async (std::launch::deferred,print_ten,'@',200);
  // deferred "get" (perform the actual calls):
  foo.get();
  bar.get();
  std::cout << std::endl;

  //expected output:
  //  with launch::deferred:
  //  **********@@@@@@@@@@
} 
```

아래는 `std::async`를 이용해 비동기적으로 `std::vector`가 가진 원소의 합을 구하는 예이다.
```cpp
template <typename T>
int parallel_sum(T beg, T end)
{
  auto len = end - beg;
  if(len < 1000)
    return std::accumulate(beg, end, 0);

  T mid = beg + len/2;
  auto handle = std::async(std::launch::async, parallel_sum<T>, mid, end);
  int sum = parallel_sum(beg, mid);
  return sum + handle.get();
}
int main() {
  std::vector<int> v(10000, 1);
  std::cout << "The sum is " << parallel_sum(v.begin(), v.end()) << '\n';
}
```
*더 읽을 거리: `std::shared_future`, `std::promise`*

### Parallel Versions of STL Algorithms (C++17)
stl containers와 관련된 알고리즘에 대한 병렬 연산 지원 문법이 추가된다. (아직 구체적인 spec이 확정되지는 않았다.)

```cpp
std::vector<int> v;

std::sort(par, v.begin(), v.end()); //parallel
std::sort(seq, v.begin(), v.end()); //sequential
```

## Other Features
### User Defined Literals 
사용자가 직접 리터럴 형식을 정의할 수 있다.
```cpp
constexpr long double operator"" _deg (long double deg) {
  return deg * 3.141592 / 180;
}
int main() {
  double x= 90.0_deg;
  std::cout << std::fixed << x <<std::endl; // output: 1.570796
}
```
C++14에서는 standard literal 정의가 추가되었다.

```cpp
auto str = "hello world"s;  //auto deduces std::string
auto dur = 60s;             //auto deduces std::chrono::seconds;
auto z = 1i                 // auto deduces std::complex<double>

int n = 0b011110            // binary literal
```

### Digit Separators (C++14)
큰 숫자의 경우 자리수를 쉽게 인지할 수 있도록 seperator를 추가할 수 있다.
```cpp
auto million = 1'000'000;
auto this_also_works = 1'0'0'000'00;
```

### Nested Namespaces (C++17)
```cpp
namespace A::B::C { /* ... */ } 
//equivalent to namespace A { namespace B { namespace C { ... } } }
```

### Initializers in `if` and `switch` Statements (C++17)
```cpp
std::map<std::string, int> map;
map["hello"] = 1;

//참고: map::insert의 반환형은 std::pair<std::map<T1, T2>::iterator>, bool > 이다. 
//삽입하려는 원소가 이미 존재하는 경우에는 기존 원소의 iterator와 false를 함께 반환한다.

if(auto ret = map.insert({"hello", 3}); !ret.second) {
  std::cout<<"hello already exists with value : "<< ret.first->second <<std::endl;
}
```

```cpp
//std::unique_lock의 예 
std::mutex mtx;
if(std::unique_lock<std::mutex> l(mtx, std::try_to_lock); l.owns_lock()) {
  std::cout<<"Successfully locked the resource";
  /* ...*/
} else { }
```

`switch` statemet에서도 사용할 수 있다.

```cpp
enum Result { SUCCESS, ABORTED, /*...*/ };
std::pair<size_t, Result> writePacket() { /*... */ }

switch (auto result = writePacket(); result.second) {
  case SUCCESS: /*... */
  case ABORTED: /*... */
}
```

### `std::tuple` and Structured Binding
A fixed-size collection of heterogeneous values. (`std::pair`의 좀 더 일반화된 버전으로 볼 수 있다.) 

```cpp
std::tuple<double, char, std::string> get_student(int id)
{
  if (id == 0) return std::make_tuple(3.8, 'A', "Lisa Simpson");
  if (id == 1) return std::make_tuple(2.9, 'C', "Milhouse Van Houten");
  /*...*/
}

int main()
{
  auto student0 = get_student(0);
  std::cout << "ID: 0, "
            << "GPA: " << std::get<0>(student0) << ", " //get으로 받는 방법
            << "grade: " << std::get<1>(student0) << ", "
            << "name: " << std::get<2>(student0) << std::endl;

  double gpa1; char grade1; std::string name1;
  std::tie(gpa1, grade1, name1) = get_student(1);  //std::tie로 받는 방법
  std::cout << "ID: 1, "
            << "GPA: " << gpa1 << ", "
            << "grade: " << grade1 << ", "
            << "name: " << name1 << std::endl;
}
```

C++17에서는 'structured binding'이라 불리는 문법을 지원한다.
```cpp
auto [gpa1, grade1, name1] = get_student(1);
// 아래와 동일하다.
// double gpa1; char grade1; std::string name1;
// std::tie(gpa1, grade1, name1) = get_student(1);  
```

### Pseudo Random Number Generators (PRNG)
난수의 품질이 보장되지 않았던 기존의 `std::rand`외에 보다 품질이 높은 pseudo random numbers를 생성하는 라이브러리가 추가되었다. 난수 생성을 위한 두가지의 요소가 제공된다.
  - Random number Engines
    - `std::mersenne_twister_engine`, `std::linear_congruental_engine` 등의 알고리즘을 이용한 난수 생성을 지원한다. 널리 사용되는 난수 생성 알고리즘을 위한 predefined keyword를 제공한다. (`std::default_random_engine`, `std::mt19937`, `std::minstd_rand` 등)
    - `std::random_device` : non-deterministic random number generator. hardware entropy source를 이용한다. 하드웨어의 지원 여부에 따라 지원하지 못하는 경우가 있으며 entropy pool이 소모되면 생성된 난수의 품질이 떨어지므로 PRNG engine의 seed로 사용하는 것이 일반적이다.
  - Random Number Distributions: 생성된 난수에 대한 post-processing을 통하여 특정한 분포를 따르게 한다.
    - `std::uniform_int_distribution`, `std::geometric_distribution`, `std::exponential_distribution`, `std::normal_distribution` 등
  - 기타 제공 함수들
    - `std::seed_seq`: 특정한 정수형 데이터의 sequence를 소비하여 unsigned seed sequence를 생성한다.

```cpp
  // Seed with a real random value, if available
  std::random_device r;

  // Choose a random mean between 1 and 6
  std::default_random_engine e1(r()); // std::random_device를 시드로 이용해 난수를 생성한다.
  std::uniform_int_distribution<int> uniform_dist(1, 6);
  int mean = uniform_dist(e1);
  std::cout << "Randomly-chosen mean: " << mean << '\n';

  // Generate a normal distribution around that mean
  std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
  std::mt19937 e2(seed2); //32-bit Mersenne Twister
  std::normal_distribution<> normal_dist(mean, 2);
  std::map<int, int> hist;

  for (int n = 0; n < 10000; ++n) {
    std::cout<<normal_dist(e2);  // normal distribution을 따르는 메르센 난수를 출력한다.
  }
```

### Additional Mathmatical Functions (C++17)
`std::beta`, `std::legendre`, `std::laguerre`, `std::riemann_zeta` 등의 mathmatical special functions들이 추가되었다.

```cpp 
double binom (int n, int k) { 
  return 1 / ((n+1) * std::beta(n-k+1, k+1)); 
}
```

### `std::optional` (C++17)
Null object pattern을 대체하는 클래스 템플릿. 값이 없는 (null) 상태를 방지할 수 있다.

```cpp 
template<typename Key, typename Value>
class Lookup { 
  std::optional<Value> get(Key key); 
  /*...*/ 
};
int main() {
  Lookup<std::string, std::string> location_lookup;
  std::string location = location_lookup.get("waldo").value_or("unknown");
}
```

## Beyond C++17
C++17 표준에는 포함되지 않았지만 중요하게 논의되었으며, 다음 표준에 포함될 것이 유력한 기능들은 다음과 같다.
- Concepts: template에 제약사항(constraints)이나 공리(axioms)를 추가할 수 있게 한다.
- Modules: `#include` 대신 `import` 키워드를 이용하여 효율적으로 코드 단위를 관리할 수 있게 한다.
- Coroutines: 비선점형(non-preemptive) 멀티태스킹을 지원하는 동시성 관련 기능들이다.


