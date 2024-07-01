/**
 * @file test_fiber.cc
 * @brief 协程测试
 * @version 0.1
 * @date 2024-06-15
 */
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "fiber.h"

void run_in_fiber2() { std::cout << "run_in_fiber2" << std::endl; }

void run_in_fiber() {
    std::cout << "run_in_fiber" << std::endl;
    coro::Fiber::GetThis()->yield();
    // fiber结束之后会自动返回主协程运行
}

void test_fiber() {
    // 初始化线程主协程
    coro::Fiber::GetThis();

    coro::Fiber::ptr fiber(new coro::Fiber(run_in_fiber, 0, false));
    fiber->resume();

    /**
     * 关于fiber智能指针的引用计数为3的说明：
     * 一份在当前函数的fiber指针，一份在MainFunc的cur指针
     * 还有一份在在run_in_fiber的GetThis()结果的临时变量里
     */
    std::cout << "fiber.use_count(): " << fiber.use_count() << std::endl;
    fiber->resume();

    fiber->reset(
        run_in_fiber2);  // 上一个协程结束之后，复用其栈空间再创建一个新协程
    fiber->resume();
}

int main(int argc, char *argv[]) {
    test_fiber();
    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 0;
}