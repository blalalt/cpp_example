#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <future>
#include <vector>
#include <string>

#include "ThreadPool.hpp"

int main() {
    // 创建一个能够并发执行四个线程的线程池
    ThreadPool pool{4};
    // 创建并发执行线程的结果列表
    std::vector<std::future<std::string>> results;

    // 启动八个需要执行的线程任务
    for (int i=0; i<8; i++) {
        // 将并发执行任务的返回值添加到结果列表中
        results.emplace_back(
            pool.enqueue(
                [i] {
                    std::cout << "hello " << i << std::endl;
                    // 等待一秒
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::cout << "world " << i << std::endl;

                    return std::string("----------thread ") + std::to_string(i) + std::string(" finished--------");
                }
            )
        );
    }
    // 输出线程执行结果
    for (auto&& result : results) {
        std::cout << result.get() << ' ';
    }
    std::cout << std::endl;
}