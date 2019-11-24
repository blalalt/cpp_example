## 0. 任务描述
将markdown解析为html。
对 Markdown 的解析，其实就是编译过程中的词法分析和语法分析阶段，而最后的到 HTML 的转换，实际上就是『目标代码的生成』这个阶段。
#### 任务关键

1. #就是 Markdown 的标志性关键字，它的个数直接决定了标题的类型，因此，我们可以非常简单的直接将其转换成 ``<h>`` 类型的 HTML 标签；

2. 正常的段落，没有任何标志性的说明，可以直接用 ``<p>`` 进行包裹；

3. ``[](), ![]()`` 这两个语法能够让我们在 Markdown 文件中插入超链接和图片，因此可以分别使用 ``<a href=" ">...</a> 和 <img alt="" src="" />``

4. 两个 `` ` 可以包裹一个行内的代码，而三个 ` `` 则可以包裹一块代码段，可以分别使用 `` <code> 和 <pre><code> `` 来进行替换

5. ``>`` 这个关键字用来处理引用的内容，可以使用`` <blockquote> ``标签

6. -, 1. 这些列表形式的关键字，可以使用 ``<ul> 和 <ol>`` 进行包裹，最后将整个内容使用 ``<li>`` 进行包装。

7. _ 和 * 这两个关键字分别有进行斜体和加粗强调的作用，在 HTML 中对应 ``<em> 和 <strong>`` 两个标签。


## 1. markdown 基础
```md
## 标题

# 一级标题
## 二级标题
### 三级标题

## 强调
- **粗体**
- _斜体_

## 引用
> 引用内容

## 列表
- 列表内容7
- 列表内容8
- 列表内容9
1. 列表内容10
2. 列表内容11
3. 列表内容12

## 代码

代码的插入方式 `printf('\n');`
\```
function method()
{
    alert("javascript");
}
\```
## 链接
行内连接 [github](https://github.com/) 的链接

## 图片
![shiyanlou logo](https://static.shiyanlou.com/img/logo_03.png)
```
它的大部分词法规则都位于行首,唯一需要考虑的，就是诸如链接、图片、代码、粗体、斜体等这些特殊的标记。


## 2. std::map


#### 初始化
1. 直接赋值
```c++
map<string, int> m;
m["a"] = 2;
```
2. insert添加
```c++
map<string, int> m;
m.insert(make_pair("a", 2));
m.insert(pair<string, int>("a", 2));
```
3. 列表初始化
```c++
map<string,int> m = {
    {"s1",1}, {"s2",2}, {"s3",3}
};
```

#### 操作
```c++
// 修改

```

## 3. std::pair

pair是将2个数据组合成一个数据，当需要这样的需求时就可以使用pair，如stl中的map就是将key和value放在一起来保存。另一个应用是，当一个函数需要返回2个数据的时候，可以选择pair。 pair的实现是一个结构体，主要的两个成员变量是first second 因为是使用struct不是class，所以可以直接使用pair的成员变量。

#### pair 的构造
1. ``template pair make_pair(T1 a, T2 b) { return pair(a, b); }``
```c++
std::make_pair(1, 1.1);
```

2. ``template <class T1, class T2> struct pair``
```  c++   
pair<int, double> p1;  //使用默认构造函数
pair<int, double> p2(1, 2.4);  //用给定值初始化
pair<int, double> p3(p2);  //拷贝构造函数
```

#### 操作
```  c++   
pair<int, double> p(1, 2.4); 
p.first == 1; // 取第一个元素
p.second == 2; // 取第二个元素
pair<int, double> p1; // 赋值
p1 = make_pair(1, 1.1);
```