# 知识点


## 1. json
#### 1.1 什么是 JSON ？
* JSON 指的是 JavaScript 对象表示法（JavaScript Object Notation）
* JSON 是轻量级的文本数据交换格式
* JSON 独立于语言 *
* JSON 具有自我描述性，更易理解

JSON 使用 JavaScript 语法来描述数据对象，但是 JSON 仍然独立于语言和平台。JSON 解析器和 JSON 库支持许多不同的编程语言。

```json
{
"employees": [
        { "firstName":"Bill" , "lastName":"Gates" },
        { "firstName":"George" , "lastName":"Bush" },
        { "firstName":"Thomas" , "lastName":"Carter" }
    ]
}
```

#### 1.2 相比 XML 的不同之处:
* 没有结束标签
* 更短
* 读写的速度更快
* 能够使用内建的 JavaScript eval() 方法进行解析
* 使用数组
* 不使用保留字

#### 1.3 JSON 语法规则
JSON 语法是 JavaScript 对象表示法语法的子集。

* 数据在名称/值对中
* 数据由逗号分隔
* 花括号保存对象
* 方括号保存数组

JSON 名称/值对
JSON 数据的书写格式是：名称/值对。

名称/值对包括字段名称（在双引号中），后面写一个冒号，然后是值：
```json
"firstName" : "John"
```

#### 1.4 JSON 值

JSON 值可以是：

* 数字（整数或浮点数）
* 字符串（在双引号中）
* 逻辑值（true 或 false）
* 数组（在方括号中）
* 对象（在花括号中）
* null
## 1.5 json 数据结构类型
#### 1.5.1 JSON 对象
JSON 对象在花括号中书写：

对象可以包含多个名称/值对：
```json
{ "firstName":"John" , "lastName":"Doe" }
```

#### 1.5.2 JSON 数组
JSON 数组在方括号中书写：

数组可包含多个对象： 
```json
{
"employees": [
        { "firstName":"Bill" , "lastName":"Gates" },
        { "firstName":"George" , "lastName":"Bush" },
        { "firstName":"Thomas" , "lastName":"Carter" }
    ]
}
```

## 2.常用的 c++ json 库
* rapidjson
* jsoncpp
* JSON++


## 3. error实参不依赖模板参数
Write()不依赖模板参数T，为无依赖名; B <T> 依赖模板参数，为依赖名。

编译原则：在查找无依赖名Write()时，编译器并不会在依赖类B <T> 中查找。

因此，编译器找不到函数Write()。
#### 解决办法
1. 将Write() -> this->Wirte()
2. 将Write() -> JsonWirter<T>::Wirte()
> https://isocpp.org/wiki/faq/templates

## 4. stack
stack\<T\>容器适配器中的数据是以 LIFO 的方式组织的.
源代码
```c++
template<typename _Tp, typename _Sequence = deque<_Tp> >
    class stack {...}
```
stack 容器适配器的模板有两个参数。第一个参数是存储对象的类型，第二个参数是底层容器的类型。stack\<T\> 的底层容器默认是 deque\<T\> 容器，因此模板类型其实是 stack\<typename T, typename Container=deque\<T\>\>。通过指定第二个模板类型参数，可以使用任意类型的底层容器，只要它们支持 back()、push_back()、pop_back()、empty()、size() 这些操作。

#### 操作
和其他序列容器相比，stack 是一类存储机制简单、所提供操作较少的容器。下面是 stack 容器可以提供的一套完整操作：
* top()：返回一个栈顶元素的引用，类型为 T&。如果栈为空，返回值未定义。
* push(const T& obj)：可以将对象副本压入栈顶。这是通过调用底层容器的 push_back() 函数完成的。
* push(T&& obj)：以移动对象的方式将对象压入栈顶。这是通过调用底层容器的有右值引用参数的 push_back() 函数完成的。
* pop()：弹出栈顶元素。
* size()：返回栈中元素的个数。
* empty()：在栈中没有元素的情况下返回 true。
* emplace()：用传入的参数调用构造函数，在栈顶生成对象。
* swap(stack\<T\> & other_stack)：将当前栈中的元素和参数中的元素交换。参数所包含元素的类型必须和当前栈的相同。对于 stack 对象有一个特例化的全局函数 swap() 可以使用。

stack\<T\> 模板也定义了复制和移动版的 operator=() 函数，因此可以将一个 stack 对象赋值给另一个 stack 对象。stack 对象有一整套比较运算符。比较运算通过字典的方式来比较底层容器中相应的元素。字典比较是一种用来对字典中的单词进行排序的方式。依次比较对应元素的值，直到遇到两个不相等的元素。第一个不匹配的元素会作为字典比较的结果。如果一个 stack 的元素比另一个 stack 的多，但是所匹配的元素都相等，那么元素多的那个 stack 容器大于元素少的 stack 容器。