/**
 * @file noncopyable.h
 * @brief 不可拷贝对象封装
 * @author shawn
 * @date 2024-05-31
 */

#ifndef __CORO_NONCOPYABLE_H__
#define __CORO_NONCOPYABLE_H__

namespace coro {

/**
 * @brief 对象无法拷贝,赋值
 */
class Noncopyable {
   public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable operator=(const Noncopyable&) = delete;
};

}  // namespace coro

#endif