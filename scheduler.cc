/**
 * @file scheduler.cc
 * @brief 协程调度器实现
 * @version 0.1
 * @date 2021-06-15
 */
#include "scheduler.h"

namespace coro {

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_scheduler_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) {
    m_useCaller = use_caller;
    m_name = name;
    if (use_caller) {
        --threads;
        t_scheduler = this;
    } else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}
}  // namespace coro