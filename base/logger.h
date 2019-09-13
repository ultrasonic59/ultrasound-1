#ifndef BASE_LOGGER_H_
#define BASE_LOGGER_H_

#include <stdarg.h>
#include <string>
#include <mutex>
#include <fstream>

#include "base/macros.h"

namespace base {

enum LOG_LEVEL {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
};

class LoggerSettings {
public:
    virtual ~LoggerSettings() {}

    virtual LOG_LEVEL GetLogLevel() = 0;
    virtual std::string GetLogFilePathLocal8Bit() = 0;
    virtual std::string GetLogFilePathUtf8() = 0;
};

class Logger {
public:
    SINGLETON_DEFINE(Logger);

    bool Initialize(LoggerSettings* settings);
    void Shutdown();

    void Log(LOG_LEVEL log_level, const char *format, ...);
    void Log(LOG_LEVEL log_level, const char* module, const char *format, ...);

    static const char* GetLogLevelText(LOG_LEVEL level);

private:
    Logger() = default;
    ~Logger() = default;

    void Logva(LOG_LEVEL log_level, const char* module, const char *format, va_list args);
    std::string GenerateTimeDateFilename();
    std::string CurrentTimeString();
    void OutputDebug(const char* str);

    inline void LogString(std::fstream &logFile, const char *timeString, char *str) {
        logFile << timeString << str << std::endl;
    }

    bool m_initialized = false;
    LoggerSettings* m_settings;
    std::string m_log_file;
    std::fstream m_log_file_stream;
    std::wstring wide_buf;
    std::mutex wide_mutex;
};

} // namespace log

#define LOG_DEBUG(format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_DEBUG, format, ##__VA_ARGS__);
#define LOG_INFO(format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_INFO, format, ##__VA_ARGS__);
#define LOG_WARNING(format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_WARNING, format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_ERROR, format, ##__VA_ARGS__);
#define LOG_FATAL(format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_FATAL, format, ##__VA_ARGS__);

#define LOG_DEBUG_M(module, format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_DEBUG, module, format, ##__VA_ARGS__);
#define LOG_INFO_M(module, format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_INFO, module, format, ##__VA_ARGS__);
#define LOG_WARNING_M(module, format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_WARNING, module, format, ##__VA_ARGS__);
#define LOG_ERROR_M(module, format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_ERROR, module, format, ##__VA_ARGS__);
#define LOG_FATAL_M(module, format, ...) \
    base::Logger::GetInstance()->Log(base::LOG_LEVEL_FATAL, module, format, ##__VA_ARGS__);

#endif // BASE_LOGGER_H_
