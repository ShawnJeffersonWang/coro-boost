/**
 * @file fiber.h
 * @brief 协程封装
 * @author shawn
 * @date 2019-05-24
 */
#ifndef __CORO_FIBER_H__
#define __CORO_FIBER_H__

#include <ucontext.h>

#include <functional>
#include <memory>

namespace coro {
class Fiber : public std::enable_shared_from_this<Fiber> {
   public:
    typedef std::shared_ptr<Fiber> ptr;

    /**
     * @brief 协程状态
     */
    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT,
    };

   private:
    /**
     * @brief 无参构造函数
     * @attention 每个线程第一个协程的构造
     */
    Fiber();

   public:
    /**
     * @brief 构造函数
     * @param[in] cb 协程运行的函数
     * @param[in] stacksize 协程栈大小
     * @param[in] use_caller 是否在MainFiber上调度
     */
    Fiber(std::function<void()> cb, size_t stacksize = 0,
          bool use_caller = false);

    ~Fiber();

    void reset(std::function<void()> cb);

    /**
     * @brief 将当前协程切换到运行状态
     * @pre getState() != EXEC
     * @post getState() = EXEC
     */
    void swapIn();

    void swapOut();

    void call();

    void back();

    uint64_t getId() const { return m_id; }

    State getState() const { return m_state; }

   public:
    /**
     * @brief 设置当前线程的运行协程
     * @param[in] f 运行协程
     */
    static void SetThis(Fiber* f);

    static ptr GetThis();

    static void YidleToHold();

    static uint64_t TotalFibers();

    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程主协程
     */
    static void MainFunc();

    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程调度协程
     */
    static void CallerMainFunc();

    /**
     * @brief 获取当前协程的id
     */
    static uint64_t GetFiberId();

   private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;
    // 协程上下文
    ucontext_t m_ctx;
    void* m_stack = nullptr;
    std::function<void()> m_cb;
};
}  // namespace coro

#endif