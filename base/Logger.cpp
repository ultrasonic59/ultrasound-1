#include "base/logger.h"

#include <time.h>
#include <chrono>
#include <windows.h>

namespace base {

bool Logger::Initialize(LoggerSettings* settings) {
    if (m_initialized)
        return true;

    m_settings = settings;

    m_log_file = m_settings->GetLogFilePathLocal8Bit();
    m_log_file += "/";
    m_log_file += GenerateTimeDateFilename();

    m_log_file_stream.open(m_log_file,
        std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

    m_initialized = true;

    return true;
}

void Logger::Shutdown() {
    m_log_file_stream.close();

    m_initialized = false;
}

void Logger::Log(LOG_LEVEL log_level, const char *format, ...) {
    if (m_log_file_stream.is_open())
        return;

    if (log_level < m_settings->GetLogLevel())
        return;

    va_list args;
    va_start(args, format);
    Logva(log_level, nullptr, format, args);
    va_end(args);
}

void Logger::Log(LOG_LEVEL log_level, const char* module, const char *format, ...) {
    if (!m_log_file_stream.is_open())
        return;

    if (log_level < m_settings->GetLogLevel())
        return;

    va_list args;
    va_start(args, format);
    Logva(log_level, module, format, args);
    va_end(args);
}

void Logger::Logva(LOG_LEVEL log_level, const char* module, const char *format, va_list args) {
    char msg[4096];
    vsnprintf(msg, 4095, format, args);

    char full_msg[4096 + 128];
    snprintf(full_msg, 4096 + 128, "[%s] [%s]: %s", GetLogLevelText(log_level), module, msg);

    OutputDebug(full_msg);

    char* str = full_msg;
    char *nextLine = str;
    std::string timeString = CurrentTimeString();
    timeString += " ";

    while (*nextLine) {
        char *nextLine = strchr(str, '\n');
        if (!nextLine)
            break;

        if (nextLine != str && nextLine[-1] == '\r') {
            nextLine[-1] = 0;
        }
        else {
            nextLine[0] = 0;
        }

        LogString(m_log_file_stream, timeString.c_str(), str);
        nextLine++;
        str = nextLine;
    }

    LogString(m_log_file_stream, timeString.c_str(), str);
}

std::string Logger::GenerateTimeDateFilename()
{
    time_t    now = time(0);
    char      file[256] = {};
    struct tm *cur_time;

    cur_time = localtime(&now);
    snprintf(file, sizeof(file), "%d-%02d-%02d%c%02d-%02d-%02d.%s",
        cur_time->tm_year + 1900,
        cur_time->tm_mon + 1,
        cur_time->tm_mday,
        '_',
        cur_time->tm_hour,
        cur_time->tm_min,
        cur_time->tm_sec,
        "txt");

    return std::string(file);
}

std::string Logger::CurrentTimeString()
{
    struct tm  tstruct;
    char       buf[80];

    auto tp = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::to_time_t(tp);
    tstruct = *localtime(&now);

    size_t written = strftime(buf, sizeof(buf), "%X", &tstruct);
    if (std::ratio_less<std::chrono::system_clock::period, std::chrono::seconds::period>::value &&
        written && (sizeof(buf) - written) > 5) {
        auto tp_secs =
            std::chrono::time_point_cast<std::chrono::seconds>(tp);
        auto millis =
            std::chrono::duration_cast<std::chrono::milliseconds>(tp - tp_secs).count();

        snprintf(buf + written, sizeof(buf) - written, ".%03u",
            static_cast<unsigned>(millis));
    }

    return buf;
}

void Logger::OutputDebug(const char* str) {
    if (IsDebuggerPresent()) {
        int wNum = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
        if (wNum > 1) {
            std::lock_guard<std::mutex> lock(wide_mutex);
            wide_buf.reserve(wNum + 1);
            wide_buf.resize(wNum - 1);
            MultiByteToWideChar(CP_UTF8, 0, str, -1, &wide_buf[0],
                wNum);
            wide_buf.push_back('\n');

            OutputDebugStringW(wide_buf.c_str());
        }
    }
}

const char* Logger::GetLogLevelText(LOG_LEVEL level) {
    switch (level)
    {
    case LOG_LEVEL_DEBUG:
        return "Debug";
    case LOG_LEVEL_INFO:
        return "Info";
    case LOG_LEVEL_WARNING:
        return "Warning";
    case LOG_LEVEL_ERROR:
        return "Error";
    case LOG_LEVEL_FATAL:
        return "Fatal";
    default:
        break;
    }
    return "";
}

} // namespace base