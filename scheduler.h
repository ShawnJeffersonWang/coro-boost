/**
 * @file scheduler.h
 * @brief 协程调度器实现
 * @version 0.1
 * @date 2024-06-15
 */

#ifndef __CORO_SCHEDULER_H__
#define __CORO_SCHEDULER_H__

#include <list>

#include "fiber.h"

namespace coro {

class Scheduler {
   public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    Scheduler(size_t threads = 1, bool use_caller = true,
              const std::string& name = "Scheduler");

    virtual ~Scheduler();

    const std::string& getName() const { return m_name; }

   private:
    std::string m_name;
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads;
    std::list<SchedulerTask> m_tasks;
    std::vector<int> m_threadIds;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount = {0};
    std::atomic<size_t> m_idleThreadCount = {0};

    bool m_useCaller;
    Fiber::ptr m_rootFiber;
    int m_rootThread = 0;

    bool m_stopping = false;
};
}  // namespace coro

#endif