#include <iostream>
#include <stdbool.h>
#include <stdint.h>

namespace myns {
int fac(int n) { return (n > 1) ? n * fac(n - 1) : 1; }

// struct [[clang::annotate("my annotation2")]] Foo {
struct Foo {
    bool c;
    int a;

 private:
    double b;
} __attribute__((annotate("my annotation")));

class Foo2 {
 public:
    int a2;
    double b2;
} __attribute__((annotate("my annotation")));

class Base {
 public:
    int baseInt;
};

class Base2 {
 public:
    int baseInt;
} __attribute__((annotate("my annotation")));

namespace myinnerns {
class Bar : public Base {
    double c;
    Foo f;
    //};
} __attribute__((annotate("my annotation")));

class Bar2 : public Base2 {
    double c;
    Foo f;
    //};
} __attribute__((annotate("my annotation")));

enum unscopedE { val1, val2 = 3, val3 = 2, sval4 } __attribute__((annotate("my annotation")));
enum unscopedE2 { val5, val6 };
enum class scopedE { sval1, sval2 = 3, sval3 = 2, sval4 } __attribute__((annotate("my annotation")));
enum class scopedEu : uint8_t { sval1, sval2 = 3, sval3 = 2, sval4 } __attribute__((annotate("my annotation")));

int main() {
    uint32_t a = 1;
    std::cout << "hello world " << a << "\n";
    return 0;
}
} // namespace myinnerns
} // namespace myns
