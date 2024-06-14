#ifndef __CORO_MUTEX_H__
#define __CORO_MUTEX_H__

#include <pthread.h>

namespace coro {

class SpinLock {};

class RWMutex {
   private:
    // 读写锁
    pthread_rwlock_t m_lock;

   public:
    RWMutex() { pthread_rwlock_init(&m_lock, nullptr); }
};

}  // namespace coro

#endif