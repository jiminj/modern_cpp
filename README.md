# Modern C++; Changes and New Features Basics

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

## Lambda Expression

![](http://www.codeguru.com/images/article/16693/lambdaparts.png)

(이미지 출처: codeguru.com)

기본적인 rules :
- [] : capture
- () : parameters
- {} : body

return type은 생략될 때가 많다. 이 때, return type은 auto deduction rule을 따른다.

```cpp
  int one = 1;
  auto addOne = [one](int n){return one + n;};
  //이 때의 type은 std::function<int(int)> addOne 이다.
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
  std::cout<< square(2)<<std::endl; //4
  std::cout<< square(3.14)<<std::endl; //9.8596
  std::cout<< square(std::complex<double>(3,-2))<<std::endl; //(5, -12)
```


## Move Semantics


## Compile-time Validation

## Concurrency

## STL Containers


