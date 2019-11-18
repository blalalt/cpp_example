#知识点

## 1. 什么是AOP
Aspect-oriented Programming,面向方面编程。传统的面向对象中的继承是一种从上而下的关系，不适合定义从左到右的横向关系，如果继承体系中的很多无关联对象都有一些公共行为，这些公共行为可能分散在不同的组建、不同的对象之中，通过继承方式提取这些公共行为不太合适。

AOP可以通过预编译方式和运行期动态代理实现在不修改源代码情况下给程序动态统一添加功能的一种技术。

假设把应用程序想成一个立体结构的话，OOP的利刃是纵向切入系统，把系统划分为很多个模块（如：用户模块，文章模块等等），而AOP的利刃是横向切入系统，提取各个模块可能都要重复操作的部分（如：权限检查，日志记录等等）。由此可见，AOP是OOP的一个有效补充。
注意：AOP不是一种技术，实际上是编程思想。凡是符合AOP思想的技术，都可以看成是AOP的实现。

> https://blog.csdn.net/pi9nc/article/details/8859303



## 2. AOP的作用
将日志记录，性能统计，安全控制，事物处理，异常处理等代码从业务逻辑代码中划分出来，通过将这些统一的面向某个方面的动作行为的分离，进而起到低耦合的效果。


## 3. AOP的实现方式
实现AOP的技术分为：静态织入和动态织入。静态织入采用专门的语法创建方面，从而使编译器在编译期间织入方面的代码。参考ASpectC++。

另一种是动态织入，一般采用动态代理的方式在运行期对方法进行拦截，将切面动态织入到方法中，可以通过代理模式来实现。见example代码

通过代理的方式实现AOP存在以下缺点：
* 不够灵活，不能自由组合多个切面。
* 耦合性强，每个切面必须从基类继承，并实现基类的接口

较好的办法是将切面作为模板的参数，这个参数是可变的，支持1到N切面，先执行核心逻辑之前的切面逻辑，再执行核心逻辑，最后执行核心逻辑之后的切面逻辑。这里通过可变参数模板支持切面的组合。为了降低耦合性，通过模板作为约束，即每个切面对象必须有Before(Args ...)或After(Args ...)方法，用来处理核心逻辑执行前后的非核心逻辑。

## 4. std::is_same 和 std::decay

#### std::is_same
位于头文件<type_traits>中

这个结构体作用很简单，就是两个一样的类型会返回true。特别在模板里面，在不清楚模板的参数时，此功能可以对一些特定的参数类型进行特殊的处理。

std::is_same对int和const int\int &\const int&等都是区别对待的，但在写模板函数时，经常会强制指定常引用进行传参，以免进行数据拷贝，这时候is_same就做出了不相等的判断，但是有时候其实我们还是希望TYPE和const TYPE& 是能认为是一样的，这时就需要std::decay进行退化处理
#### std::decay 退化类型的修饰
其实就是把各种引用啊什么的修饰去掉，把cosnt int&退化为int，这样就能通过std::is_same正确识别出加了引用的类型了

* 在模板里可以通过std::is_same判断模板的类型，从而实现对不同类型的区别对待
* 在堆类型要求不是非常严格的情况下，可以使用std::decay把类型退化为基本形态，结合std::is_same用，可以判断出更多的情况

>版权声明：本文为CSDN博主「尘中远」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/czyt1988/article/details/52812797

## 5. std::declval和std::decltype
std::declval的定义如下：
```c++
template<class T>
typename std::add_rvalue_reference<T>::type declval() noexcept;
```
将任何一个类型T转换成右值引用类型，使得在decltype表达式中不必经过构造函数就能使用成员函数。
```c++
#include <utility>
#include <iostream>

struct Default { int foo() const { return 1; } };

struct NonDefault
{
    NonDefault(const NonDefault&) { }
    int foo() const { return 1; }
};

int main()
{
    decltype(Default().foo()) n1 = 1;                   // type of n1 is int
//  decltype(NonDefault().foo()) n2 = n1;               // error: no default constructor
    decltype(std::declval<NonDefault>().foo()) n2 = n1; // type of n2 is int
    std::cout<< "n1 = " << n1 << '\n'
    std::cout<< "n2 = " << n2 << '\n';
}
```

给定一个变量或表达式，decltype能够推导出它的结果类型。最重要的是不需要计算表达式就能推导出表达式所得值的类型。

## 6. std::enable_if
定义：
```c++
template <bool, typename T=void>
struct enable_if {
};
 
template <typename T>
struct enable_if<true, T> {
  using type = T;
};
```
满足条件时类型有效。只有当第一个模板参数为 true 时，type 才有定义，否则使用 type 会产生编译错误，并且默认模板参数可以不必指定类型。
作用：
#### 用法一：类型偏特化
在使用模板编程时，经常会用到根据模板参数的某些特性进行不同类型的选择，或者在编译时校验模板参数的某些特性。例如：

```c++
template <typename T, typename Enable=void>
struct check;
 
template <typename T>
struct check<T, typename std::enable_if<T::value>::type> {
  static constexpr bool value = T::value;
};
```
上述的 check 只希望选择 value==true 的 T，否则就报编译时错误。

#### 用法二：控制函数返回类型
对于模板函数，有时希望根据不同的模板参数返回不同类型的值，进而给函数模板也赋予类型模板特化的性质。典型的例子可以参看 tuple 的获取第 k 个元素的 get 函数：
```c++
template <std::size_t k, class T, class... Ts>
typename std::enable_if<k==0, typename element_type_holder<0, T, Ts...>::type&>::type
get(tuple<T, Ts...> &t) {
  return t.tail; 
}
 
template <std::size_t k, class T, class... Ts>
typename std::enable_if<k!=0, typename element_type_holder<k, T, Ts...>::type&>::type
get(tuple<T, Ts...> &t) {
  tuple<Ts...> &base = t;
  return get<k-1>(base); 
}
```
由于函数模板不能偏特化，通过 enable_if 便可以根据 k 值的不同情况选择调用哪个 get，进而实现函数模板的多态。

#### 用法三：校验函数模板参数类型
有时定义的模板函数，只希望特定的类型可以调用，参考 cppreference 官网示例，很好的说明了如何限制只有整型可以调用的函数定义：
```c++

template <typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type
is_odd(T t) {
  return bool(t%2);
}
 
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
bool is_even(T t) {
  return !is_odd(t); 
}
```
一个通过返回值，一个通过默认模板参数，都可以实现校验模板参数是整型的功能。
> https://blog.csdn.net/jeffasd/article/details/84667090

## 7. std::forward，万能引用和完美转发
涉及到型别推导的T&&类型是万能引用，不涉及到型别推导的就是右值引用。
```c++
void f(Widget&& param); //右值引用
template <typename T>
void f(T&& param);  //万能引用
```
万能引用可以绑定到左值引用，右值引用，const或者非const对象，volatile或者非volatile对象。

万能引用代表的是什么类型，取决于初始化形参的实参类型，当传入左值的时候，万能引用代表左值类型，若传入右值则代表右值类型。

万能引用对格式要求比较严格，必须是T&&类型的型别推导。

传入实参的时候，是一个右值，而当传入的值绑定到a的时候，这个值就变成了一个左值。这时候内部的函数就无法分辨传入的值是左值还是右值了。所以需要完美转发左右值到内部函数调用。

当传入的类型是左值引用的时候，返回左值引用，但传入的类型是右值引用的时候，返回右值引用。但是传入的类型如果是普通的T呢，这里也是会强转为右值类型的。也就是说，除了传入左值引用外，传入别的类型（右值引用，普通值类型）都返回右值引用。
```c++
int a = 3;
forward<int &&>(a); //返回右值
forward<int &>(a);  //返回左值
forward<int>(a);    //返回右值
```
> https://blog.csdn.net/u012630961/article/details/81016126 ,推荐阅读
