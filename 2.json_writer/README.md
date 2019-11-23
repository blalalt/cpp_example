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
