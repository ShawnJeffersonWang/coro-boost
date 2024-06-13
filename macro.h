/**
 * @file macro.h
 * @brief 常用宏的封装
 * @author shawn
 * @date 2024-06-01
 */

#ifndef __CORO_MACRO_H__
#define __CORO_MACRO_H__

#include <assert.h>
#include <string.h>

#include "log.h"

#define CORO_UNLIKELY(x) __builtin_expect(!!(x), 0)

// 断言宏封装
#define CORO_ASSERT(x)                                  \
    if (CORO_UNLIKELY(!(x))) {                          \
        CORO_LOG_ERROR(CORO_LOG_ROOT())                 \
            << "ASSERTION: " #x << "\nbacktrace:\n"     \
            << coro::BacktraceToString(100, 2, "    "); \
        assert(x);                                      \
    }

// 断言宏封装
#define CORO_ASSERT2(x, w)                              \
    if (CORO_UNLIKELY(!(x))) {                          \
        CORO_LOG_ERROR(CORO_LOG_ROOT())                 \
            << "ASSERTION: " #x << "\n"                 \
            << w << "\nbacktrace:\n"                    \
            << coro::BacktraceToString(100, 2, "    "); \
        assert(x);                                      \
    }

#endif