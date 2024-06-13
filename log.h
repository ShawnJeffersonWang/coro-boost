/**
 * @file log.h
 * @brief 日志模块封装
 * @author shawn
 * @date 2024-05-23
 */
#ifndef __CORO_LOG_H__
#define __CORO_LOG_H__

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "mutex.h"
#include "singleton.h"

#define CORO_LOG_LEVEL(logger, level)                                  \
    if (logger->getLevel() <= level)                                   \
    coro::LogEventWrap(                                                \
        coro::LogEvent::ptr(new coro::LogEvent(                        \
            logger, level, __FILE__, __LINE__, 0, coro::GetThreadId(), \
            coro::GetFiberId(), time(0), coro::Thread::GetName())))    \
        .getSS()

#define CORO_LOG_ERROR(logger) CORO_LOG_LEVEL(logger, coro::LogLevel::ERROR)

/**
 * @brief 获取name的日志器
 */
#define CORO_LOG_NAME(name) coro::LoggerMgr::GetInstance()->getLogger(name)

/**
 * @brief 获取主日志器
 */
#define CORO_LOG_ROOT() coro::LoggerMgr::GetInstance()->getRoot()

namespace coro {

class Logger;
class LoggerManager;

/**
 * @brief 日志级别
 */
class LogLevel {
   public:
    enum Level {
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    /**
     * @brief 将日志级别转成文本输出
     * @param[in] level 日志级别
     */
    static const char* ToString(LogLevel::Level level);

    /**
     * @brief 将文本转换成日志级别
     * @param[in] str 日志级别文本
     */
    static LogLevel::Level FromString(const std::string& str);
};

/**
 * @brief 日志事件
 */
class LogEvent {
   public:
    typedef std::shared_ptr<LogEvent> ptr;
};

class LogFormatter {
   public:
    typedef std::shared_ptr<LogFormatter> ptr;
};

class LogAppender {
    friend class Logger;

   public:
    typedef std::shared_ptr<LogAppender> ptr;
    typedef SpinLock MutexType;
};

/**
 * @brief 日志器
 */
class Logger {
    friend class LoggerManager;

   public:
    typedef std::shared_ptr<Logger> ptr;
    typedef SpinLock MutexType;

    Logger(const std::string& name = "root");

    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);

   private:
    // 日志名称
    std::string m_name;
    // 日志级别
    LogLevel::Level m_level;
    // Mutex
    MutexType m_mutex;
    // 目标日志集合
    std::list<LogAppender::ptr> m_appenders;
    // 日志格式器
    LogFormatter::ptr m_formatter;
    // 主日志器
    Logger::ptr m_root;
};

class StdoutLogAppender {};

class FileLogAppender {};

/**
 * @brief 日志器管理类
 */
class LoggerManager {
   public:
    typedef SpinLock MutexType;
    LoggerManager();
};

// 日志器管理类单例模式
typedef coro::Singleton<LoggerManager> LoggerMgr;

}  // namespace coro

#endif