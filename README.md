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

**더 읽을거리: Universal reference, `std::forward`**

## Compile-time Validation

## Concurrency

## Smart Pointers
### std::auto_ptr (C++98/deprecated)
<!--scope가 종료될 때 해제된다. 대입연산과 관련해 소유권 이전 -->

### std::shared_ptr
가장 널리 사용되는 smart pointer이다. reference counting을 이용해 수명 주기를 관리한다. 참조하고 있는 scope를 이용해 참조 횟수를 관리하며, 값이 0이 되면 자동으로 해제된다. 
### std::weak_ptr
`std::shared_ptr` 또는 같은 `std::weak_ptr`로부터만 생성이 가능하다. `std::shared_ptr`의 reference count를 증가시키지 않음으로써, 잠재적인 circular reference의 위험에서 벗어날 수 있다. 객체의 수명주기에 영향을 미치지 않으면서 값을 참조하고 싶을 때 쓰일 수 있다.
### std::unique_ptr (boost::scoped_ptr)
`std::auto_ptr`의 대체재로 도입되었다. `std::move`를 이용한다.

## STL Containers


