// 线程池头文件
#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <queue>
#include <vector>
#include <functional>
#include <condition_variable>


class ThreadPool {
public:
    // 在线程池创建 num_threads 个线程
    ThreadPool(size_t num_threads);

    // 向线程池增加线程
    template <typename F, typename ... Args>
    auto enqueue(F&& f, Args&& ... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    ~ThreadPool();
private:

    // 时刻追踪线程以保证可以使用join
    std::vector<std::thread> workers;
    //任务队列
    std::queue<std::function<void()>> tasks;

    // 同步相关
    std::mutex queue_mutex; //互斥锁
    std::condition_variable condition; // 互斥条件变量

    // 停止相关
    bool stop{false};
};

/* 
构造函数：
编写线程池的构造函数的过程中，要注意这样几点：

1.构造的线程池应该执行的是指定的可并发执行的线程的数量；
2.任务的执行和结束阶段应该是位于临界区的，这样才能保证不会并发的同时启动多个需要执行的任务；
*/
// 构造函数只负责启动一定数量的工作线程(worker)
inline ThreadPool::ThreadPool(size_t num_threads) {
    // 启动 num_threads 数量的线程
    for (size_t i=0; i<num_threads; ++i) {
        this->workers.emplace_back(
            // 捕获 this
            [this] {
                // 循环避免虚假唤醒，并不断从tasks中获取任务
                for (;;) {
                    // 定义函数对象的容器, 存储任意的返回类型为 void 参数表为空的函数
                    // 用来接收task
                    std::function<void()> task;

                    // 临界区
                    {
                        // 创建互斥锁
                        std::unique_lock<std::mutex> lock(this->queue_mutex);

                        // 阻塞当前线程直到 condition_variable 被唤醒
                        this->condition.wait(lock, [this]{return this->stop || !this->tasks.empty();});


                        // 如果当前线程池已经结束或者等待任务队列为空，则直接退出
                        if (this->stop && this->tasks.empty()) {
                            return ;
                        }

                        // 否则就取出等待任务队列的队首任务
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    task(); // 执行任务
                }
            }
        );
    }
}

/*
线程池的销毁对应构造时究竟创建了什么实例，销毁线程池之前，我们并不知道当前线程池中的工作线程是否执行完成，
所以必须先创建一个临界区将线程池状态标记为停止，从而禁止新的线程的加入，
最后等待所有执行线程的运行结束，完成销毁
*/
// 销毁所有线程池中创建的线程
inline ThreadPool::~ThreadPool() {
    // 临界区
    {
        // 创建互斥锁哦
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        
        // 设置线程池状态
        this->stop = true;
    }

    // 通知所有的等待线程
    this->condition.notify_all();

    // 使所有异步线程转为同步执行

    for(auto& work: this->workers) {
        work.join();
    }
}

/*
向线程池中添加新任务的实现逻辑主要需要注意这样几点：

1.支持多个入队任务参数需要使用变长模板参数
2.为了调度执行的任务，需要包装执行的任务，这就意味着需要对任务函数的类型进行包装、构造
3.临界区可以在一个作用域里面被创建，最佳实践是使用 RAII 的形式
*/
// 添加一个新的线程到线程池中
template <class F, class ... Args>
auto ThreadPool::enqueue(F&& f, Args&& ... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    
    // 推断返回类型
    using return_type = typename std::result_of<F(Args...)>::type;

    // 获得当前任务
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    // 获取std::futurex对象 以供实施线程同步
    std::future<return_type> res = task->get_future();

    // 临界区
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex);

        // 禁止在线程池停止后加入新的线程
        if (this->stop) {
            throw std::runtime_error("ThreadPool ended.");
        }

        // 将线程加入到执行任务队列
        tasks.emplace([task]{(*task)();});
    }

    // 通知一个等待的线程
    this->condition.notify_one();

    return res;
}