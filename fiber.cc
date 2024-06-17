#include "fiber.h"

#include <ucontext.h>

#include <atomic>
#include <cstddef>

#include "log.h"
#include "macro.h"

// UV: Unique Visitors（独立的访问者数）
// RPS: Requests Per Second（每秒请求数）
namespace coro {

static Logger::ptr g_logger = CORO_LOG_NAME("system");

// 全局静态变量，用于生成协程id
static std::atomic<uint64_t> s_fiber_id{0};
// 全局静态变量，用于统计当前的协程数
static std::atomic<uint64_t> s_fiber_count{0};

// 线程局部变量，当前线程正在运行的协程
static thread_local Fiber* t_fiber = nullptr;
// 线程局部变量，当前线程的主协程，切换到这个协程，就相当于切换到了主线程中运行，智能指针形式
static thread_local Fiber::ptr t_thread_fiber = nullptr;

static ConfigVar<uint32_t>::ptr g_fiber_stack_size = Config::Lookup<uint32_t>(
    "fiber.stack_size", 128 * 1024, "fiber stack size");

/**
 * @brief 构造函数
 * @attention
 * 无参构造函数只用于创建线程的第一个协程，也就是线程主函数对应的协程，
 * 这个协程只能由GetThis()方法调用，所以定义成私有方法
 */
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);

    // ucp: use context pointer，即用户上下文指针
    if (getcontext(&m_ctx)) {
        CORO_ASSERT2(false, "getcontext");
    }
    s_fiber_count++;
    CORO_LOG_DEBUG(g_logger) << "Fiber::Fiber() main id = " << m_id;
}

/**
 * @brief 构造函数，用于创建用户协程
 * @param[] cb 协程入口函数
 * @param[] stacksize 栈大小，默认为128k
 */
Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    : m_id(s_fiber_id++), m_cb(cb) {
    s_fiber_count++;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);

    if (getcontext(&m_ctx)) {
        CORO_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    CORO_LOG_DEBUG(g_logger) << "Fiber::Fiber() id = " << m_id;
}

/**
 * @brief 返回当前线程正在执行的协程
 * @details 如果当前线程还未创建协程，则创建线程的第一个协程，
 * 且该协程为当前线程的主协程，其他协程都通过这个协程来调度，也就是说，其他协程
 * 结束时,都要切回到主协程，由主协程重新选择新的协程进行resume
 * @attention
 * 线程如果要创建协程，那么应该首先执行一下Fiber::GetThis()操作，以初始化主函数协程
 */
Fiber::ptr GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    CORO_ASSERT(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::resume() {
    CORO_ASSERT(m_state != TERM && m_state != RUNNING);
    SetThis(this);
    m_state = RUNNING;

    if (swapcontext(&(t_thread_fiber->m_ctx), &m_ctx)) {
        CORO_ASSERT2(false, "swapcontext");
    }
}

void Fiber::yield() {
    //
    CORO_ASSERT(m_state == RUNNING || m_state == TERM);
    SetThis(t_thread_fiber.get());
    if (m_state != TERM) {
        m_state = READY;
    }
    if (swapcontext(&m_ctx, &(t_thread_fiber->m_ctx))) {
        CORO_ASSERT2(false, "swapcontext");
    }
}
}  // namespace coro