/**
 * @file scheduler.cc
 * @brief 协程调度器实现
 * @version 0.1
 * @date 2024-06-15
 */
#include "scheduler.h"

#include <cassert>

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_scheduler_fiber = nullptr;
static thread_local int s_thread_id = 0;

// 获取调度器指针
Scheduler* Scheduler::GetThis() { return t_scheduler; }

void Scheduler::SetThis() { t_scheduler = this; }

Fiber* Scheduler::GetSchedulerFiber() { return t_scheduler_fiber; }

int Scheduler::GetThreadId() { return s_thread_id; }

void Scheduler::SetThreadId(int thread_id) { s_thread_id = thread_id; }

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

Scheduler::~Scheduler() {}

void Scheduler::start() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_stopping) {
        std::cerr << "Scheduler is stopped" << std::endl;
        return;
    }

    assert(m_threads.empty());
    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; i++) {
    }
}

void Scheduler::run() {
    std::cout << "Scheduler::run() starts in thread: " << GetThreadId()
              << std::endl;
    SetThis();
    if (GetThreadId() != m_rootThread) {
        assert(t_scheduler_fiber == nullptr);
        t_scheduler_fiber = Fiber::GetThis().get();
    }
}

void Scheduler::tickle() { tickler++; }

// 发布线程任务
void Scheduler::scheduleLock(std::shared_ptr<Fiber> fc, int thread_id) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        SchedulerTask task;

        task.fiber = fc;
        task.cb = nullptr;
        task.thread = thread_id;

        m_tasks.push_back(task);
    }
    tickle();
}

// 发布函数任务
void Scheduler::scheduleLock(std::function<void()> fc, int thread_id) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        SchedulerTask task;

        task.fiber = nullptr;
        task.cb = fc;
        task.thread = thread_id;

        m_tasks.push_back(task);
    }
    tickle();
}