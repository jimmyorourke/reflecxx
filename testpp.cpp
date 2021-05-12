#include <iostream>
#include <stdint.h>
#include <stdbool.h>

namespace myns{
int fac(int n) {
    return (n>1) ? n*fac(n-1) : 1;
}

//struct [[clang::annotate("my annotation2")]] Foo {
struct Foo {
    bool c;
    int a;
private:
    double b;
}__attribute__((annotate("my annotation")));

class Foo2 {
public:
    int a2;
    double b2;
}__attribute__((annotate("my annotation")));

class Base {
public:
    int baseInt;
};

class Base2 {
public:
    int baseInt;
}__attribute__((annotate("my annotation")));

namespace myinnerns {
class Bar : public Base {
    double c;
    Foo f;
//};
}__attribute__((annotate("my annotation")));

class Bar2 : public Base2 {
    double c;
    Foo f;
//};
}__attribute__((annotate("my annotation")));

enum unscoped {val1, val2 = 3, val3=2};
enum class scoped {sval1, sval2 = 3, sval3=2};
enum class scopedu : uint8_t {sval1, sval2 = 3, sval3=2};

int main() {
    uint32_t a = 1;
    std::cout << "hello world " << a << "\n";
    return 0;
}
}
}
