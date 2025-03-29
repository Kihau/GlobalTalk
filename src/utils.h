#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

#define ansi_reset  "\x1b[0m"
#define ansi_fatal  "\x1b[41;1;30m"
#define ansi_red    "\x1b[1;31m"
#define ansi_green  "\x1b[1;32m"
#define ansi_yellow "\x1b[1;33m"
#define ansi_blue   "\x1b[1;34m"
#define ansi_purple "\x1b[1;35m"

#include <cstdio>
// #define log_error(...) fprintf(stderr, "ERROR: "  __VA_ARGS__);
#define log_fatal(format, ...)   fprintf(stderr, ansi_fatal  "[FATAL] " ansi_reset  format "\n" __VA_OPT__(,) __VA_ARGS__)
#define log_error(format, ...)   fprintf(stderr, ansi_red    "[ERROR] " ansi_reset  format "\n" __VA_OPT__(,) __VA_ARGS__)
#define log_warning(format, ...) fprintf(stdout, ansi_yellow "[WARN ] " ansi_reset  format "\n" __VA_OPT__(,) __VA_ARGS__)
#define log_info(format, ...)    fprintf(stdout, ansi_blue   "[INFO ] " ansi_reset  format "\n" __VA_OPT__(,) __VA_ARGS__)
#define log_debug(format, ...)   fprintf(stdout, ansi_purple "[DEBUG] " ansi_reset format "\n" __VA_OPT__(,) __VA_ARGS__)

// #define internal static

#define mark_unused(variable) (void)variable

#include <cassert>
#define not_implemented() assert(false && "Not implemented.")

//
// Defer macro thingy
//
template <typename T> class ScopeDrop {
    T lambda;
    
public:
    ScopeDrop(T lambda) : lambda(lambda) {}
    ~ScopeDrop() { lambda(); }
    
private:
    ScopeDrop(const ScopeDrop &);
    ScopeDrop &operator=(const ScopeDrop &);
};

class ScopeDropCreate {
public:
    template <typename T> ScopeDrop<T> operator+(T t) { return t; }
};

#define _concatenate(x, y) x##y
#define _defer_internal(line) _concatenate(_defer_, line)
#define defer const auto &_defer_internal(__LINE__) = ScopeDropCreate() + [&]()

#endif // !UTILS_H
