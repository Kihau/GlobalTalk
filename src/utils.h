#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

using i8  = int8_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

#include <cstdio>
// #define log_error(...) fprintf(stderr, "ERROR: "  __VA_ARGS__);
#define log_error(format, ...)   fprintf(stderr, "ERROR: "   format "\n" __VA_OPT__(,) __VA_ARGS__)
#define log_warning(format, ...) fprintf(stdout, "WARNING: " format "\n" __VA_OPT__(,) __VA_ARGS__)
#define log_info(format, ...)    fprintf(stdout, "INFO: "    format "\n" __VA_OPT__(,) __VA_ARGS__)

#define mark_unused(variable) (void)variable;

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
