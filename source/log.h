////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2018, Openex Intelligence Technology (ShangHai) Co., Ltd.
/// All rights reserved.
///
/// @file    log.h
/// @brief   Logging util
/// @version 1.0
/// @author  Shawn Lee
/// @date    2018-7-1
///
/// Revision: Initial version.
///
////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <sstream>
#include <chrono>

namespace utils {
    enum class log_priority {
        error,
        warn,
        info,
        detail
    };

    bool log_init(const std::string & path, log_priority threshold);
    void log_set_threshold(log_priority threshold);

    class log_service;
    class log_obj {
        friend class log_service;

    private:
        explicit log_obj(log_priority priority) : 
            priority_(priority) {}

    public:
        static log_obj & error(const char * file, const char * function, int line);
        static log_obj & warn(const char * file, const char * function, int line);
        static log_obj & info(const char * file, const char * function, int line);
        static log_obj & detail(const char * file, const char * function, int line);
        static void debug(const char * format, ...);
        static void debug(const std::string & str);
        static void debug_va(const char * format, va_list & args);
        static void debug(const char * func, int line, const char * format, ...);
        static void debug(const char * func, int line, const std::string & str);
        static void debug_va(const char * func, int line, const char * format, va_list & args);
        log_obj & prefix(const char * file, const char * function, int line);

    public:
        log_obj & operator()(const char * fmt, ...);
        log_obj & operator()(const std::string & str);
        log_obj & va(const char * fmt, va_list & args);

        template<class T>
        friend log_obj & operator << (log_obj & obj, const T & data) {
            if (auto s = stream()) {
                *s << data;
                flush();
            }

            return *this;
        }

        log_obj & flush();

    private:
        std::ostringstream * stream();

    private:
        log_priority priority_;
    };

    class profiler {
    public:
        int ms(bool reset = true) {
            auto now = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();

            if (reset) {
                start_ = now;
            }

            return (int)diff;// .count();
        }

        int us(bool reset = true) {
            auto now = std::chrono::high_resolution_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - start_).count();

            if (reset) {
                start_ = now;
            }

            return (int)diff;// .count();
        }

        void reset() {
            start_ = std::chrono::high_resolution_clock::now();
        }

    private:
        std::chrono::high_resolution_clock::time_point start_ =
            std::chrono::high_resolution_clock::now();
    };
}


#define log_error(...) do { utils::log_obj::error(__FILE__, __FUNCTION__, __LINE__)(##__VA_ARGS__);} while(0)
#define log_warn(...) do { utils::log_obj::warn(__FILE__, __FUNCTION__, __LINE__)(##__VA_ARGS__);} while(0)
#define log_info(...) do { utils::log_obj::info(__FILE__, __FUNCTION__, __LINE__)(##__VA_ARGS__);} while(0)
#define log_detail(...) do { utils::log_obj::detail(__FILE__, __FUNCTION__, __LINE__)(##__VA_ARGS__);} while(0)

#define log_debug(...) utils::log_obj::debug(__FUNCTION__, __LINE__, ##__VA_ARGS__ )
