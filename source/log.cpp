////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2018, Openex Intelligence Technology (ShangHai) Co., Ltd.
/// All rights reserved.
///
/// @file    log.cpp
/// @brief   Logging util
/// @version 1.0
/// @author  Shawn Lee
/// @date    2018-7-1
///
/// Revision: Initial version.
///
////////////////////////////////////////////////////////////////////////

#pragma once


#include "log.h"

#include <cstdarg>
#include <cassert>
#include <cstdlib>
#include <sstream>

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//#include "system.h"
//#include "filesystem.h"

namespace utils {
    class log_service {
    public:
        static log_service & instance() {
            static log_service the_instance;
            return the_instance;
        }

    private:
        log_service() = default;

    public:
        ~log_service() {
            finalize();
        }
    public:
        bool initialize(const std::string & path, log_priority threshold) {
            assert(file_ == nullptr);

            threshold_ = threshold;
            file_ = fopen(path.c_str(), "w");
            return file_ != nullptr;
        }

        void finalize() {
            if (file_) {
                fclose(file_);
                file_ = nullptr;
            }
        }

        void set_threshold(log_priority threshold) {
            threshold_ = threshold;
        }

        void print(log_priority priority, const char * fmt, ...) {
            if (priority <= threshold_ && file_) {
                va_list args;
                va_start(args, fmt);
                vfprintf_s(file_, fmt, args);
                fputs("", file_);
                va_end(args);
                fflush(file_);
            }
        }

        void print_va(log_priority priority, const char * fmt, va_list args) {
            if (priority <= threshold_ && file_) {
                vfprintf_s(file_, fmt, args);
                fputs("", file_);
                fflush(file_);
            }
        }

        void print(log_priority priority, const std::string & str) {
            if (priority <= threshold_ && file_) {
                fputs(str.c_str(), file_);
                fflush(file_);
            }
        }

        void flush() {
            if (oss_.tellp() > 0 && file_) {
                fputs(oss_.str().c_str(), file_);
                oss_.str({}); 
                fflush(file_);
            }
        }

        std::ostringstream * stream(log_priority priority) {
            if (priority <= threshold_) {
                return &oss_;
            }

            return nullptr;
        }

    private:
        log_priority threshold_ = log_priority::detail;
        std::ostringstream oss_;
        FILE * file_ = nullptr;
    };
}

namespace utils {
    bool log_init(const std::string & path, log_priority threshold) {
        //utils::fs::create_directories(utils::fs::parent(path));
        return log_service::instance().initialize(path, threshold);
    }

    void log_set_threshold(log_priority threshold) {
        log_service::instance().set_threshold(threshold);
    }

    log_obj & log_obj::error(const char * file, const char * function, int line) {
        static log_obj instance(utils::log_priority::error);
        return instance.prefix(file, function, line);
    }

    log_obj & log_obj::warn(const char * file, const char * function, int line) {
        static log_obj instance(utils::log_priority::warn);
        return instance.prefix(file, function, line);
    }

    log_obj & log_obj::info(const char * file, const char * function, int line) {
        static log_obj instance(utils::log_priority::info);
        return instance.prefix(file, function, line);
    }

    log_obj & log_obj::detail(const char * file, const char * function, int line) {
        static log_obj instance(utils::log_priority::detail);
        return instance.prefix(file, function, line);
    }

    void log_obj::debug(const char * format, ...) {
#ifdef _DEBUG
        va_list args;
        va_start(args, format);
        debug_va(format, args);
        va_end(args);
#endif
    }

    void log_obj::debug(const std::string & str) {
#ifdef _DEBUG
        OutputDebugStringA(str.c_str());
        std::cout << str;
#endif
    }

    void log_obj::debug_va(const char * format, va_list & args) {
#ifdef _DEBUG
        char buffer[9192] = { 0 };

        vsprintf_s(buffer, format, args);
        OutputDebugStringA(buffer);
        std::cout << buffer;
#endif
    }

    void log_obj::debug(const char * func, int line, const char * format, ...) {
#ifdef _DEBUG
        va_list args;
        va_start(args, format);
        debug_va(func, line, format, args);
        va_end(args);
#endif
    }

    void log_obj::debug(const char * func, int line, const std::string & str) {
#ifdef _DEBUG
        char prefix[1024] = { 0 };
        sprintf(prefix, "%s[%d] ", func, line);
        OutputDebugStringA(prefix);
        OutputDebugStringA(str.c_str());
        std::cout << prefix << str;
#endif
    }

    void log_obj::debug_va(const char * func, int line, const char * format, va_list & args) {
#ifdef _DEBUG
        char buffer[9192] = { 0 };
        char prefix[1024] = { 0 };
        sprintf_s(prefix, "%s[%d] ", func, line);
        OutputDebugStringA(prefix);
        std::cout << prefix;
        vsprintf_s(buffer, format, args);
        OutputDebugStringA(buffer);
        std::cout << buffer;

#endif
    }

    log_obj & log_obj::prefix(const char * file, const char * function, int line) {
        SYSTEMTIME  time;

        GetLocalTime(&time);

        if (auto p = strrchr(file, '\\')) {
            file = p + 1;
        }

        log_service::instance().print(priority_, 
                                      "%04hu-%02hu-%02hu %02hu:%02hu:%02hu:%03hu %s[%d]:%s ",
                                      time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
                                      file, line, function);
        debug("%04hu-%02hu-%02hu %02hu:%02hu:%02hu:%03hu %s[%d]:%s ",
              time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
              file, line, function);
        return *this;
    }



    log_obj &  log_obj::operator()(const char * fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log_service::instance().print_va(priority_, fmt, args);
        va_end(args);

        va_start(args, fmt);
        debug_va(fmt, args);
        va_end(args);
        return *this;
    }
    log_obj & log_obj::va(const char * fmt, va_list & args) {
        log_service::instance().print_va(priority_, fmt, args);
        debug_va(fmt, args);
        return *this;
    }

    log_obj & log_obj::operator()(const std::string & str) {
        log_service::instance().print(priority_, str);
        debug(str);
        return *this;
    }

    std::ostringstream * log_obj::stream() {
        return log_service::instance().stream(priority_);
    }

    log_obj & log_obj::flush() {
        log_service::instance().flush();
        return *this;
    }   

};


