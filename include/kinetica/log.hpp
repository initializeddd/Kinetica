#ifndef KINETICA_LOG_HPP
#define KINETICA_LOG_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>

// ---- Terminal detection ----
#ifdef _WIN32
#include <io.h>
#define K_IS_TERMINAL _isatty(_fileno(stdout))
#else
#include <unistd.h>
#define K_IS_TERMINAL isatty(STDOUT_FILENO)
#endif

// ---- C++20 source_location support ----
#if __cplusplus >= 202002L && !defined(__apple_build_version__) // Apple Clang lags
#include <source_location>
#define K_HAVE_SOURCE_LOCATION 1
#else
#define K_HAVE_SOURCE_LOCATION 0
#endif

// ---- enable timestamps ----
#define K_LOG_TIMESTAMPS

namespace Kinetica::Log {

    // ---- ANSI color definitions (your brand palette) ----
    namespace Color {
        constexpr const char* reset     = "\033[0m";
        constexpr const char* kinetica  = "\033[38;2;154;128;214m"; // #9A80D6
        constexpr const char* debug     = "\033[38;2;0;255;157m";    // #00FF9D
        constexpr const char* info      = "\033[38;2;120;200;255m"; // soft blue
        constexpr const char* warn      = "\033[38;2;255;200;85m";  // amber
        constexpr const char* error     = "\033[38;2;255;85;85m";   // soft red
    }

    // ---- Helper: current time as HH:MM:SS.mmm ----
    #ifdef K_LOG_TIMESTAMPS
    inline std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
        return std::string(buf) + "." + std::to_string(ms.count());
    }
    #endif

    // ---- Core logging function (C++20 version) ----
    #if K_HAVE_SOURCE_LOCATION
    template<typename... Args>
    inline void log_impl(const char* level, const char* color,
                         const std::string& fmt, std::source_location loc = std::source_location::current()) {
        std::ostringstream oss;
        #ifdef K_LOG_TIMESTAMPS
        oss << "[" << timestamp() << "] ";
        #endif
        oss << fmt;

        if (K_IS_TERMINAL) {
            std::cout << Color::kinetica << "[ KINETICA ] " << Color::reset
            << color << level << Color::reset << " "
            << loc.file_name() << ":" << loc.line()
            << " - " << oss.str() << "\n";
        } else {
            std::cout << "[ KINETICA ] " << level << " "
            << loc.file_name() << ":" << loc.line()
            << " - " << oss.str() << "\n";
        }
                         }
                         #endif

                         // ---- Core logging function (pre-C++20 fallback) ----
                         template<typename... Args>
                         inline void log_impl(const char* level, const char* color,
                                              const std::string& fmt, const char* file, int line) {
                             std::ostringstream oss;
                             #ifdef K_LOG_TIMESTAMPS
                             oss << "[" << timestamp() << "] ";
                             #endif
                             oss << fmt;

                             if (K_IS_TERMINAL) {
                                 std::cout << Color::kinetica << "[ KINETICA ] " << Color::reset
                                 << color << level << Color::reset << " "
                                 << file << ":" << line
                                 << " - " << oss.str() << "\n";
                             } else {
                                 std::cout << "[ KINETICA ] " << level << " "
                                 << file << ":" << line
                                 << " - " << oss.str() << "\n";
                             }
                                              }

} // namespace Kinetica::Log

// ---- Macros: DEBUG/INFO/WARN/ERROR ----

#ifdef KINETICA_DEBUG_LOG
// Debug builds: all levels enabled
#if K_HAVE_SOURCE_LOCATION
#define KLOG_DEBUG(fmt) ::Kinetica::Log::log_impl("DEBUG", ::Kinetica::Log::Color::debug, fmt)
#define KLOG_INFO(fmt)  ::Kinetica::Log::log_impl("INFO",  ::Kinetica::Log::Color::info,  fmt)
#define KLOG_WARN(fmt)  ::Kinetica::Log::log_impl("WARN",  ::Kinetica::Log::Color::warn,  fmt)
#define KLOG_ERROR(fmt) ::Kinetica::Log::log_impl("ERROR", ::Kinetica::Log::Color::error, fmt)
#else
#define KLOG_DEBUG(fmt) ::Kinetica::Log::log_impl("DEBUG", ::Kinetica::Log::Color::debug, fmt, __FILE__, __LINE__)
#define KLOG_INFO(fmt)  ::Kinetica::Log::log_impl("INFO",  ::Kinetica::Log::Color::info,  fmt, __FILE__, __LINE__)
#define KLOG_WARN(fmt)  ::Kinetica::Log::log_impl("WARN",  ::Kinetica::Log::Color::warn,  fmt, __FILE__, __LINE__)
#define KLOG_ERROR(fmt) ::Kinetica::Log::log_impl("ERROR", ::Kinetica::Log::Color::error, fmt, __FILE__, __LINE__)
#endif
#else
// Release builds: only WARN/ERROR
#if K_HAVE_SOURCE_LOCATION
#define KLOG_DEBUG(fmt) (void)0
#define KLOG_INFO(fmt)  (void)0
#define KLOG_WARN(fmt)  ::Kinetica::Log::log_impl("WARN",  "", fmt)
#define KLOG_ERROR(fmt) ::Kinetica::Log::log_impl("ERROR", "", fmt)
#else
#define KLOG_DEBUG(fmt) (void)0
#define KLOG_INFO(fmt)  (void)0
#define KLOG_WARN(fmt)  ::Kinetica::Log::log_impl("WARN",  "", fmt, __FILE__, __LINE__)
#define KLOG_ERROR(fmt) ::Kinetica::Log::log_impl("ERROR", "", fmt, __FILE__, __LINE__)
#endif
#endif

// ---- Formatted logging helper (C++17 and above) ----
// Usage: KLOG_DEBUG_F("Loaded {} models in {:.2f}s", count, elapsed);
#if __cplusplus >= 201703L
#include <fmt/core.h>
#undef KLOG_DEBUG
#undef KLOG_INFO
#undef KLOG_WARN
#undef KLOG_ERROR

#ifdef KINETICA_DEBUG_LOG
#if K_HAVE_SOURCE_LOCATION
#define KLOG_DEBUG(fmt, ...) ::Kinetica::Log::log_impl("DEBUG", ::Kinetica::Log::Color::debug, fmt::format(fmt, __VA_ARGS__))
#define KLOG_INFO(fmt, ...)  ::Kinetica::Log::log_impl("INFO",  ::Kinetica::Log::Color::info,  fmt::format(fmt, __VA_ARGS__))
#define KLOG_WARN(fmt, ...)  ::Kinetica::Log::log_impl("WARN",  ::Kinetica::Log::Color::warn,  fmt::format(fmt, __VA_ARGS__))
#define KLOG_ERROR(fmt, ...) ::Kinetica::Log::log_impl("ERROR", ::Kinetica::Log::Color::error, fmt::format(fmt, __VA_ARGS__))
        #else
            #define KLOG_DEBUG(fmt, ...) ::Kinetica::Log::log_impl("DEBUG", ::Kinetica::Log::Color::debug, fmt::format(fmt, __VA_ARGS__), __FILE__, __LINE__)
            #define KLOG_INFO(fmt, ...)  ::Kinetica::Log::log_impl("INFO",  ::Kinetica::Log::Color::info,  fmt::format(fmt, __VA_ARGS__), __FILE__, __LINE__)
            #define KLOG_WARN(fmt, ...)  ::Kinetica::Log::log_impl("WARN",  ::Kinetica::Log::Color::warn,  fmt::format(fmt, __VA_ARGS__), __FILE__, __LINE__)
            #define KLOG_ERROR(fmt, ...) ::Kinetica::Log::log_impl("ERROR", ::Kinetica::Log::Color::error, fmt::format(fmt, __VA_ARGS__), __FILE__, __LINE__)
        #endif
    #else
        #if K_HAVE_SOURCE_LOCATION
            #define KLOG_DEBUG(fmt, ...) (void)0
            #define KLOG_INFO(fmt, ...)  (void)0
            #define KLOG_WARN(fmt, ...)  ::Kinetica::Log::log_impl("WARN",  "", fmt::format(fmt, __VA_ARGS__))
            #define KLOG_ERROR(fmt, ...) ::Kinetica::Log::log_impl("ERROR", "", fmt::format(fmt, __VA_ARGS__))
        #else
            #define KLOG_DEBUG(fmt, ...) (void)0
            #define KLOG_INFO(fmt, ...)  (void)0
            #define KLOG_WARN(fmt, ...)  ::Kinetica::Log::log_impl("WARN",  "", fmt::format(fmt, __VA_ARGS__), __FILE__, __LINE__)
            #define KLOG_ERROR(fmt, ...) ::Kinetica::Log::log_impl("ERROR", "", fmt::format(fmt, __VA_ARGS__), __FILE__, __LINE__)
        #endif
    #endif
#endif // C++17

#endif // KINETICA_LOG_HPP
