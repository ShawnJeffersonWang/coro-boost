/**
 * @file util.h
 * @brief 常用的工具函数
 * @author shawn
 * @date 2024-05-27
 */
#ifndef __CORO_UTIL_H__
#define __CORO_UTIL_H__

#include <string>
#include <vector>

namespace coro {

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

std::string BacktraceToString(int size = 64, int skip = 2,
                              const std::string& prefix = "");
}  // namespace coro

#endif