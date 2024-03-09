#include <cstdlib>

template <typename T> class ScopeDrop {
public:
    T lambda;
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

int main(int argc, char **argv) {
    char *data = (char *)malloc(1024);
    defer { free(data); };

    return 0;
}
