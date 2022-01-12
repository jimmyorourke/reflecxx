#include <libtest_types/structs.hpp>

#include <sstream>
#include <string>

// This cpp is mainly used for defining a target whose dependencies and compilation flags will be used by libclang
// during the code generation.
// We can also demonstrate that this target can still make use of the generated code here in the cpp, as only the
// headers are parsed by the generator.

namespace test_types {
namespace {
// This is a super naive toString implementation that assumes all members are ostreamable. However that's sufficient for
// BasicStruct and to demonstrate making use of the generated visitors from this compilation unit.
template <typename T>
std::string dumbToString(const T& obj) {
    std::ostringstream out;
    bool first = true;
    auto v = [&out, &first](const char* name, const auto& member) {
        if (!first) {
            out << "\n";
        }
        first = false;
        out << name << ":" << member;
    };
    reflecxx::visit(obj, std::move(v));
    return out.str();
}
} // namespace

std::string toString(const BasicStruct& b) {
    // The fact that this compiles shows that we are able to use the generated visitor of BasicStruct.
    return dumbToString(b);
}

// Demonstrate that we can make use of generated visitors without requiring any ifdef protection here in the cpp.
bool BasicStruct::operator==(const BasicStruct& rhs) const {
    return this->b == rhs.b && this->i == rhs.i && this->d == rhs.d;
    // return reflecxx::equalTo(*this, rhs);
}

} // namespace test_types
