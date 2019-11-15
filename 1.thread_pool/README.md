# 知识点
```shell
#运行
g++ main.cpp -std=c++11 -pthread
```

## 0. 线程池
多线程技术主要是解决单个处理器单元内多个线程的执行问题，由此诞生了所谓的线程池技术。线程池主要由三个基本部分组成：

* 线程池管理器（Thread Pool）：负责创建、管理线程池，最基本的操作为：创建线程池、销毁线程池、增加新的线程任务；
* 工作线程（Worker）：线程池中的线程，在没有任务时会处于等待状态，可以循环执行任务；
* 任务队列（Tasks Queue）：未处理任务的缓存队列。

简单来说， **一个线程池负责管理了需要执行的多个并发执行的多个线程中可执行数量多的线程、以及他们之间的调度。**

有了线程池之后，我们可以在程序启动后创建一定数量的线程，当任务到达后，缓冲队列会将任务加入到线程中进行执行，执行完成后，线程并不销毁，而是等待下一任务的到来。



## 1. hpp中的enqueue声明
```c++
template <typename F, typename ... Args>
    auto enqueue(F&& f, Args&& ... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
```
在这个定义中，设计的是一个可变参数的模板，使用 typename... 可以指出接下来的参数表示零个或者多个类型的列表，并使用 Args 来表示，这个 Args 是一个模板参数包。而在后面的函数参数表中，args 则是函数参数包，用来表示零个或多个参数。

设计的 enqueue 函数会让一个新的线程被传入进去，在第一个参数中使用 F&& 来进行右值引用，而不会发生拷贝行为而再次创建一个执行体，其返回类型，我们设计成获得要执行函数体在执行完毕后返回的结果，这时候就必须推断出返回类型是什么，才能写出 std::future\<typename 返回类型\> 的代码。

而实际上执行函数的返回类型是可以被 std::result_of 推断的，我们只需要使用所调函数的类型F，以及这个函数的参数 Args 即可 std::result_of<F(Args...)>::type。

最后，这个定义中使用了尾置返回类型的 C++11 特性，但这里并不是因为无法写出返回类型，而是因为返回类型太长，使用尾置的形式可以让我们的代码比前置更加清晰好看


## 2. std::mutex, std::unique_lock
C++11引入了 mutex 相关的类，其所有相关的函数都放在 \<mutex\> 头文件中。

std::mutex 是 C++11 中最基本的 mutex 类，通过实例化 std::mutex 可以创建互斥量，而通过其成员函数 lock() 可以仅此能上锁，unlock() 可以进行解锁。但是在在实际编写代码的过程中，最好不去直接调用成员函数，因为调用成员函数就需要在每个临界区的出口处调用 unlock()，当然，还包括异常。这时候 C++11 还为互斥量提供了一个 RAII 语法的模板类std::lock_gurad。RAII 在不失代码简洁性的同时，很好的保证了代码的异常安全性。
```c++
void some_operation(const std::string &message) {
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock(mutex);

    // ...操作

    // 当离开这个作用域的时候，互斥锁会被析构，同时unlock互斥锁
    // 因此这个函数内部的可以认为是临界区
}
```
由于 C++ 保证了所有栈对象在声明周期结束时会被销毁，所以这样的代码也是异常安全的。无论 some_operation() 正常返回、还是在中途抛出异常，都会引发堆栈回退，也就自动调用了 unlock()。

而 std::unique_lock 则相对于 std::lock_guard 出现的，std::unique_lock 更加灵活，std::unique_lock 的对象会以独占所有权（没有其他的 unique_lock 对象同时拥有某个 mutex 对象的所有权）的方式管理 mutex 对象上的上锁和解锁的操作。所以在并发编程中，推荐使用 std::unique_lock。例如：
```c++
#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;

void block_area() {
    std::unique_lock<std::mutex> lock(mtx);
    //...临界区
}
int main() {
    std::thread thd1(block_area);

    thd1.join();

    return 0;
}
```

## 3.std::future, std::packaged_task
std::future 则是提供了一个访问异步操作结果的途径，这句话很不好理解。为了理解这个特性，我们需要先理解一下在 C++11之前的多线程行为。

试想，如果我们的主线程 A 希望新开辟一个线程 B 去执行某个我们预期的任务，并返回我一个结果。而这时候，线程 A 可能正在忙其他的事情，无暇顾及 B 的记过，所以我们会很自然的希望能够在某个特定的时间获得线程 B 的结果。

在 C++11 的 std::future 被引入之前，通常的做法是：创建一个线程A，在线程A里启动任务 B，当准备完毕后发送一个事件，并将结果保存在全局变量中。而主函数线程 A 里正在做其他的事情，当需要结果的时候，调用一个线程等待函数来获得执行的结果。

而 C++11 提供的 std::future 简化了这个流程，可以用来获取异步任务的结果。自然地，我们很容易能够想象到把它作为一种简单的线程同步手段。

此外，std::packaged_task 可以用来封装任何可以调用的目标，从而用于实现异步的调用。例如：

```c++
#include <iostream>
#include <future>
#include <thread>

int main()
{
    // 将一个返回值为7的 lambda 表达式封装到 task 中
    // std::packaged_task 的模板参数为要封装函数的类型
    std::packaged_task<int()> task([](){return 7;});
    // 获得 task 的 future
    std::future<int> result = task.get_future();    // 在一个线程中执行 task
    std::thread(std::move(task)).detach();    std::cout << "Waiting...";
    result.wait();
    // 输出执行结果
    std::cout << "Done!" << std:: endl << "Result is " << result.get() << '\n';
}
```
在封装好要调用的目标后，可以使用 get_future() 来获得一个 std::future 对象，以便之后事实线程同步。

## 4.std::condition_variable
std::condition_variable 是为了解决死锁而生的。当互斥操作不够用而引入的。比如，线程可能需要等待某个条件为真才能继续执行，而一个忙等待循环中可能会导致所有其他线程都无法进入临界区使得条件为真时，就会发生死锁。所以，condition_variable 实例被创建出现主要就是用于唤醒等待线程从而避免死锁。std::condition_variable的 notify_one() 用于唤醒一个线程；notify_all() 则是通知所有线程。

当 std::condition_variable 对象的某个 wait 函数被调用的时候，它使用 std::unique_lock(通过 std::mutex) 来锁住当前线程。

当前线程会一直被阻塞，直到另外一个线程在相同的 std::condition_variable 对象上调用了 notification 函数来唤醒当前线程。
>https://www.cnblogs.com/xiangtingshen/p/10538833.html
```c++
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <queue>
#include <chrono>

int main()
{
    // 生产者数量
    std::queue<int> produced_nums;
    // 互斥锁
    std::mutex m;
    // 条件变量
    std::condition_variable cond_var;
    // 结束标志
    bool done = false;
    // 通知标志
    bool notified = false;

    // 生产者线程
    std::thread producer([&]() {
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // 创建互斥锁
            std::unique_lock<std::mutex> lock(m);
            std::cout << "producing " << i << '\n';
            produced_nums.push(i);
            notified = true;
            // 通知一个线程
            cond_var.notify_one();
        }   
        done = true;
        cond_var.notify_one();
    }); 

    // 消费者线程
    std::thread consumer([&]() {
        std::unique_lock<std::mutex> lock(m);
        while (!done) {
            while (!notified) {  // 循环避免虚假唤醒
                cond_var.wait(lock);
            }   
            while (!produced_nums.empty()) {
                std::cout << "consuming " << produced_nums.front() << '\n';
                produced_nums.pop();
            }   
            notified = false;
        }   
    }); 

    producer.join();
    consumer.join();
}
```

## 5. std::result_of
对于 std::result_of，它的作用是可以在编译的时候推导出一个函数调用表达式的返回值类型，例如：
```c++
struct S {
    double operator()(char, int&); // 这个函数的返回类型是 double
};

int main()
{

    std::result_of<S(char, int&)>::type foo = 3.14; // 使用这样的写法会推导出模板参数中函数的返回值类型
    typedef std::result_of<S(char, int&)>::type MyType; // 是 double 类型吗?
    std::cout << "foo's type is double: " << std::is_same<double, MyType>::value << std::endl;
    return 0;
}

// 输出:
// foo's type is double: true
```
>注：result_of在C++17中被移出，使用class invoke_result代替。

## 6. vector.emplace_back(Args ... args)
C++11中，针对顺序容器(如vector、deque、list)，新标准引入了三个新成员：emplace_front、emplace和emplace_back，这些操作构造而不是拷贝元素。这些操作分别对应push_front、insert和push_back，允许我们将元素放置在容器头部、一个指定位置之前或容器尾部。

当调用push或insert成员函数时，我们将元素类型的对象传递给它们，这些对象被拷贝到容器中。
**而当我们调用一个emplace成员函数时，则是将参数传递给元素类型的构造函数。emplace成员使用这些参数在容器管理的内存空间中直接构造元素。**

**emplace函数的参数根据元素类型而变化，参数必须与元素类型的构造函数相匹配。emplace函数在容器中直接构造元素。传递给emplace函数的参数必须与元素类型的构造函数相匹配。**

其它容器中，std::forward_list中的emplace_after、emplace_front函数，std::map/std::multimap中的emplace、emplace_hint函数，std::set/std::multiset中的emplace、emplace_hint，std::stack中的emplace函数，等emplace相似函数操作也均是构造而不是拷贝元素。

**emplace相关函数可以减少内存拷贝和移动。当插入rvalue，它节约了一次move构造，当插入lvalue，它节约了一次copy构造。**


## 7. lambda表达式
与参数传值类似，**值捕获**的前提是变量可以拷贝，不同之处则在于，被捕获的变量在 lambda 表达式被创建时拷贝，而非调用时才拷贝：
```c++
void learn_lambda_func_1() {
    int value_1 = 1;
    auto copy_value_1 = [value_1] {
        return value_1;
    };
    value_1 = 100;
    auto stored_value_1 = copy_value_1();
    // 这时, stored_value_1 == 1, 而 value_1 == 100.
    // 因为 copy_value_1 在创建时就保存了一份 value_1 的拷贝
}
```

与引用传参类似，**引用捕获保存的是引用**，值会发生变化。
* []: 空捕获列表，意味着**表达式内部无法使用其外层上下文中的任何局部名字**。其数据需要**从实参或者非局部变量(名字空间变量包括全局变量)**中获得。
* \[&]: 引用隐式捕获，所有**局部变量都通过引用访问**。
* \[=]: 通过值隐式捕获，所有名字都**指向局部变量的副本**，在lambda表达式调用点获得的。
* \[捕获列表]: 显式捕获，捕获列表通过值或引用的方式捕获的局部变量的名字列表。以&为前缀的变量名字通过引用捕获，其他通过值捕获。**捕获列表中可以出现this**。
* \[&，捕获列表]：对于名字没有出现在捕获列表中的局部变量。通过引用隐式捕获。列出的名字不能以&为前缀。**捕获列表中可以出现this**。
* [=，捕获列表]：对于名字没有出现在捕获列表中的局部变量。通过值隐式捕获。列出的名字必须以&为前缀。**捕获列表中不允许包含this**。捕获列表中的变量名通过引用的方式捕获。
* this默认是引用捕获，不管捕获列表内是&还是=，在C++17中，通过[*this]可以指定通过复制的方式捕获当前对象。
> https://docs.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=vs-2019
https://zh.cppreference.com/w/cpp/language/lambda


## 8. function 和 bind
#### function
定义在头文件 functional 里
```c++
template<typename _Res, typename... _ArgTypes>
class function<_Res(_ArgTypes...)>
```

类模板 std::function 是通用多态函数封装器。 std::function 的实例能存储、复制及调用任何可回调目标——函数、lambda表达式、bind表达式、其他函数对象，还可以指向成员函数指针和指向数据成员指针。

存储的可调用对象被称为 std::function 的目标。若 std::function 不含目标，则称它为空。调用空 std::function 的目标导致抛出 bad_function_call 异常。

function 满足可复制构造和可复制赋值。

#### bind
定义在头文件 functional 里
```c++
template<typename _Func, typename... _BoundArgs>
inline typename
_Bind_helper<__is_socketlike<_Func>::value, _Func, _BoundArgs...>::type
bind(_Func&& __f, _BoundArgs&&... __args)

template<typename _Result, typename _Func, typename... _BoundArgs>
inline
typename _Bindres_helper<_Result, _Func, _BoundArgs...>::type
bind(_Func&& __f, _BoundArgs&&... __args)
```

函数模板 bind 生成 f 的转发调用包装器。调用此包装器等价于以一些绑定到 args 的参数调用 f 。类似于 python 的 functools.partial

参数 f 表示可调用对象（函数对象、指向函数指针、函数的引用、指向成员函数指针或指向数据成员指针）

参数 __args 表示要绑定的参数列表。未绑定参数使用命名空间 std::placeholders 的占位符 _1, _2, _3... 所替换

需要注意的是：

* 调用指向非静态成员函数指针或指向非静态数据成员指针时，首参数必须是引用或指针（可以包含智能指针，如 std::shared_ptr 与 std::unique_ptr），指向将访问其成员的对象。

* 到 bind 的参数被复制或移动，而且决不按引用传递，除非包装于 ref 或 cref

* 允许同一 bind 表达式中的多重占位符（例如多个 _1 ），但结果仅若对应参数（ u1 ）是左值或不可移动右值才良好定义。

## 9. Move 和 Forward

#### 引用折叠
* A& & 折叠成 A&
* A& && 折叠成 A&
* A&& & 折叠成 A&
* A&& && 折叠成 A&&
>https://blog.csdn.net/booirror/article/details/45057689
#### Move
与move相对的就是copy。众所周知，C++的变量都是值语义的。所谓的值语义的变量，就是它的行为跟int类型是一样的，典型表现就是作为参数传入函数中需要再复制一个作为形式参数，它的变化不会影响到原来的变量。

move语义就是把旧指针的值复制到新指针，并把旧指针的值赋为NULL。既然是这么简单的事情，为什么C++11还要把它作为一个新特性呢？原因有很多，①避免程序员所有权转移不彻底（忘了写后面的p=NULL）；②让move这个语义更清晰；③增加语法支持（也就是常说的右值引用）让编译器在编译时就能做优化处理……
```c++
// FUNCTION TEMPLATE move
template<class _Ty>
	_NODISCARD constexpr remove_reference_t<_Ty>&&
		move(_Ty&& _Arg) noexcept
	{	// forward _Arg as movable
	return (static_cast<remove_reference_t<_Ty>&&>(_Arg));
	}
```
参数是右值引用类型，因此经过引用折叠，对左值和右值分别产生了左值引用和右值引用，使用remove_reference_t获取本来的不带引用的基本类型，然后static_cast为对应右值引用类型。
>https://blog.csdn.net/gg_18826075157/article/details/71889042
#### Forward
分别对左值引用和右值引用分别使用不同的模板函数,对 **int & ,int&&** 都增加了一个 &&，根据引用折叠规则，其实是没有改变其引用性质的。

对于实参而言，不论本身是左值引用还是右值引用，它是一个有名变量，所以是左值。因此，使用Forward将他作为参数传递时static_cast为本来的类型，从而保证参数的正确传递。
```c++
// FUNCTION TEMPLATE forward
template<class _Ty>
	_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>& _Arg) noexcept
	{	// forward an lvalue as either an lvalue or an rvalue
	return (static_cast<_Ty&&>(_Arg));
	}

template<class _Ty>
	_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept
	{	// forward an rvalue as an rvalue
	static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
	return (static_cast<_Ty&&>(_Arg));
	}
```
>https://blog.csdn.net/booirror/article/details/45057689

##### remove_reference_t
针对不同类型，T,T &,T &&,分别重载，定义出只返回不带引用符号的T。
```c++
// STRUCT TEMPLATE remove_reference
template<class _Ty>
	struct remove_reference
	{	// remove reference
	using type = _Ty;
	};

template<class _Ty>
	struct remove_reference<_Ty&>
	{	// remove reference
	using type = _Ty;
	};

template<class _Ty>
	struct remove_reference<_Ty&&>
	{	// remove rvalue reference
	using type = _Ty;
	};

template<class _Ty>
	using remove_reference_t = typename remove_reference<_Ty>::type;
```
> https://blog.csdn.net/qq_25327609/article/details/89533836