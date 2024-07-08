/**
 * @file scheduler.h
 * @brief 协程调度器实现
 * @version 0.1
 * @date 2024-06-15
 */

#ifndef __CORO_SCHEDULER_H__
#define __CORO_SCHEDULER_H__

#include <atomic>
#include <list>
#include <mutex>
#include <string>
#include <vector>

#include "fiber.h"

class Fiber;

class Thread;

class Scheduler {
   public:
    Scheduler(size_t threads = 1, bool use_caller = true,
              const std::string& name = "Scheduler");

    virtual ~Scheduler();

    const std::string& getName() const { return m_name; }

    static Scheduler* GetThis();
    void SetThis();

    static Fiber* GetSchedulerFiber();

    void scheduleLock(std::shared_ptr<Fiber> fc, int thread_id = -1);
    void scheduleLock(std::function<void()> fc, int thread_id = -1);

    static int GetThreadId();
    static void SetThreadId(int thread_id);

    virtual void start();
    virtual void stop();

    virtual void tickle();

   protected:
    virtual void run();

    virtual void idle();

    virtual bool stopping() { return m_stopping; }

    bool hasIdleThreads() { return m_idleThreadCount > 0; }

   private:
    // 调度任务，协程/函数二选一，可指定在哪个线程上调度
    struct SchedulerTask {
        std::shared_ptr<Fiber> fiber;
        std::function<void()> cb;
        int thread;

        SchedulerTask() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }

        SchedulerTask(std::shared_ptr<Fiber> f, int thr) {
            fiber = f;
            thread = thr;
        }

        SchedulerTask(std::shared_ptr<Fiber>* f, int thr) {
            fiber.swap(*f);
            thread = thr;
        }

        SchedulerTask(std::function<void()> f, int thr) {
            cb = f;
            thread = thr;
        }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

   private:
    // 协程调度器名称
    std::string m_name;
    // 互斥锁
    std::mutex m_mutex;
    // 线程池
    std::vector<std::shared_ptr<Thread>> m_threads;
    // 任务队列
    std::vector<SchedulerTask> m_tasks;
    // 线程池的线程ID数组
    std::vector<int> m_threadIds;
    // 工作线程的数量，不包括use_caller主线程
    size_t m_threadCount = 0;
    // 活跃的线程数
    std::atomic<size_t> m_activeThreadCount = {0};
    // idle 线程数
    std::atomic<size_t> m_idleThreadCount = {0};

    // 是否使用caller线程执行任务
    bool m_useCaller;
    // use_caller为true时，调度器所在线程的调度协程 -> 必须在类内持有
    // 不然创建完就会被释放
    std::shared_ptr<Fiber> m_rootFiber;
    // 调度器所在线程的id
    int m_rootThread = 0;
    // 用于唤醒在idle协程中的线程
    std::atomic<int> tickler;

   protected:
    bool m_stopping = false;
};

#endif