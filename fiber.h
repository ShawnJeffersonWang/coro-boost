/**
 * @file fiber.h
 * @brief 协程封装
 * @author shawn
 * @date 2024-05-24
 */
#ifndef __CORO_FIBER_H__
#define __CORO_FIBER_H__

#include <stdlib.h>
#include <ucontext.h>

#include <functional>
#include <memory>

namespace coro {

/**
 * @brief 协程类
 */
class Fiber : public std::enable_shared_from_this<Fiber> {
   public:
    typedef std::shared_ptr<Fiber> ptr;

    /**
     * @brief 协程状态
     */
    enum State { READY, RUNNING, TERM };

   private:
    /**
     * @brief 构造函数
     * @attention
     * 无参构造函数只用于创建线程的第一个协程，也就是线程主函数对应的协程，
     * 这个协程只能由GetThis()方法调用，所以定义成私有方法
     */
    Fiber();

   public:
    /**
     * @brief 构造函数，用于创建用户协程
     * @param[in] cb 协程入口函数
     * @param[in] stacksize 栈大小
     * @param[in] run_in_scheduler 本协程是否参与调度器调度，默认为true
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0,
          bool run_in_scheduler = true);

    /**
     * @brief 析构函数
     */
    ~Fiber();

    /**
     * @brief 重置协程状态和入口函数，复用栈空间，不重新创建栈
     * @param[in] cb
     */
    void reset(std::function<void()> cb);

    /**
     * @brief 将当前协程切到到执行状态
     * @details
     * 当前协程和正在运行的协程进行交换，前者状态变为RUNNING，后者状态变为READY
     */
    void resume();

    /**
     * @brief 当前协程让出执行权
     * @details
     * 当前协程与上次resume时退到后台的协程进行交换，前者状态变为READY，后者状态变为RUNNING
     */
    void yield();

    /**
     * @brief 获取协程ID
     */
    uint64_t getId() const { return m_id; }

    /**
     * @brief 获取协程状态
     */
    State getState() const { return m_state; }

   public:
    /**
     * @brief 设置当前正在运行的协程，即设置线程局部变量t_fiber的值
     */
    static void SetThis(Fiber* f);

    /**
     * @brief 返回当前线程正在执行的协程
     * @details 如果当前线程还未创建协程，则创建线程的第一个协程，
     * 且该协程为当前线程的主协程，其他协程都通过这个协程来调度，也就是说，其他协程
     * 结束时,都要切回到主协程，由主协程重新选择新的协程进行resume
     * @attention
     * 线程如果要创建协程，那么应该首先执行一下Fiber::GetThis()操作，以初始化主函数协程
     */
    static Fiber::ptr GetThis();

    /**
     * @brief 获取总协程数
     */
    static uint64_t TotalFibers();

    /**
     * @brief 协程入口函数
     */
    static void MainFunc();

    /**
     * @brief 获取当前协程id
     */
    static uint64_t GetFiberId();

   private:
    // 协程id
    uint64_t m_id = 0;
    // 协程栈大小
    uint32_t m_stacksize = 0;
    // 协程状态
    State m_state = READY;
    // 协程上下文
    ucontext_t m_ctx;
    // 协程栈地址
    void* m_stack = nullptr;
    // 协程入口函数
    std::function<void()> m_cb;
    // 本协程是否参与调度器调度
    bool m_run_in_scheduler;
};

}  // namespace coro

#endif