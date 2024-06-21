#include "fiber.h"

#include <ucontext.h>

#include <atomic>
#include <cstddef>
#include <iostream>

// UV: Unique Visitors（独立的访问者数）
// RPS: Requests Per Second（每秒请求数）
namespace coro {

// 全局静态变量，用于生成协程id
static std::atomic<uint64_t> s_fiber_id{0};
// 全局静态变量，用于统计当前的协程数
static std::atomic<uint64_t> s_fiber_count{0};

// 线程局部变量，当前线程正在运行的协程
static thread_local Fiber *t_fiber = nullptr;
// 线程局部变量，当前线程的主协程，切换到这个协程，就相当于切换到了主线程中运行，智能指针形式
static thread_local Fiber::ptr t_thread_fiber = nullptr;

// static ConfigVar<uint32_t>::ptr g_fiber_stack_size =
// Config::Lookup<uint32_t>(
//     "fiber.stack_size", 128 * 1024, "fiber stack size");
size_t default_size = 128 * 1024;

/**
 * @brief malloc栈内存分配器
 */
class MallocStackAllocator {
   public:
    static void *Alloc(size_t size) { return malloc(size); }
    static void Dealloc(void *vp, size_t size) { return free(vp); }
};

using StackAllocator = MallocStackAllocator;

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

/**
 * @brief 构造函数
 * @attention
 * 无参构造函数只用于创建线程的第一个协程，也就是线程主函数对应的协程，
 * 这个协程只能由GetThis()方法调用，所以定义成私有方法
 */
Fiber::Fiber() {
    SetThis(this);
    m_state = RUNNING;

    // ucp: use context pointer，即用户上下文指针
    if (getcontext(&m_ctx)) {
    }
    s_fiber_count++;
    m_id = s_fiber_id++;
}

void Fiber::SetThis(Fiber *f) { t_fiber = f; }

/**
 * 获取当前协程，同时充当初始化当前线程主协程的作用，这个函数在使用协程之前要调用一下
 */
Fiber::ptr Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber());
    t_thread_fiber = main_fiber;
    return t_fiber->shared_from_this();
}

/**
 * @brief 构造函数，用于创建用户协程
 * @param[] cb 协程入口函数
 * @param[] stacksize 栈大小，默认为128k
 */
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    : m_id(s_fiber_id++), m_cb(cb) {
    s_fiber_count++;
    m_stacksize = stacksize ? stacksize : default_size;
    m_stack = StackAllocator::Alloc(m_stacksize);

    if (getcontext(&m_ctx)) {
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
}

/**
 * 线程的主协程析构时需要特殊处理，因为主协程没有分配栈和cb
 */
Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        // 有栈，说明是子协程，需要确保子协程一定是结束状态
        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        // 没有栈，说明是线程的主协程

        Fiber *cur = t_fiber;  // 当前协程就是自己
        if (cur == this) {
            SetThis(nullptr);
        }
    }
}

/**
 * 这里为了简化状态管理，强制只有TERM状态的协程才可以重置，但其实刚创建好但没执行过的协程也应该允许重置的
 */
void Fiber::reset(std::function<void()> cb) {
    m_cb = cb;
    if (getcontext(&m_ctx)) {
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = READY;
}

// 子协程的resume操作一定是在主协程里执行的
void Fiber::resume() {
    std::cout << "id: " << this->GetFiberId() << " resume()" << std::endl;
    SetThis(this);
    m_state = RUNNING;
    if (m_run_in_scheduler) {
        if (swapcontext(&(t_thread_fiber->m_ctx), &m_ctx)) {
        }
    } else {
        if (swapcontext(&(t_thread_fiber->m_ctx), &m_ctx)) {
        }
    }
}

// 主协程的resume操作一定是在子协程里执行的
void Fiber::yield() {
    std::cout << "id: " << this->GetFiberId() << " yield()" << std::endl;
    SetThis(t_thread_fiber.get());
    if (m_state != TERM) {
        m_state = READY;
    }
    if (m_run_in_scheduler) {
        if (swapcontext(&m_ctx, &(t_thread_fiber->m_ctx))) {
        }
    } else {
        if (swapcontext(&m_ctx, &(t_thread_fiber->m_ctx))) {
        }
    }
}

/**
 * 这里没有处理协程函数出现异常的情况，同样是为了简化状态管理，并且个人认为协程的异常不应该由框架处理，应该由开发者自行处理
 */
void Fiber::MainFunc() {
    Fiber::ptr cur =
        GetThis();  // GetThis()的shared_from_this()方法让引用计数加1

    cur->m_cb();
    cur->m_cb = nullptr;
    cur->m_state = TERM;

    auto raw_ptr = cur.get();  // 手动让t_fiber的引用计数减1
    cur.reset();
    raw_ptr->yield();
}

}  // namespace coro