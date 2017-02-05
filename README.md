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

C++11에서도 함수의 반환형에 쓸 수 있다.
```cpp
template<class T, class U>
auto add(T t, U u) -> decltype(t + u) { //trailing return type
  return t + u;
}
```

## Lambda Expression


## Move Semantics


## Compile-time Validation

## Concurrency

## STL Containers


