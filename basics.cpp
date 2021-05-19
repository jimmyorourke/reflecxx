#include <iostream>
#include <string>
#include <type_traits>

enum class MyEnum { val1, val2, val3 };

enum class MyEnum2 { val1, val2 };

enum class MyEnum3 { val1, val2 };

template <class T>
struct dependent_false : std::false_type {};

// template <typename Visitor>
// constexpr void visitMyEnum(Visitor&& visitor) {
//   visitor(MyEnum::val1, "val1", 0);
//   visitor(MyEnum::val2, "val2", 2);
//   visitor(MyEnum::val3, "val3", 3);
// }

template <typename EnumType, typename Visitor>
struct Acceptor {
  static_assert(dependent_false<EnumType>{},
                "Must specialize the Acceptor class for each Enum Type");
  // Prototype of the functions that all specializations should implement
  static constexpr void visit(Visitor&& visitor);
};

// TODO: Generate these from the enum definitions with some wacky variadic
// macros
template <typename Visitor>
struct Acceptor<MyEnum, Visitor> {
  static constexpr void visit(Visitor&& visitor) {
    visitor(MyEnum::val1, "val1", 0);
    visitor(MyEnum::val2, "val2", 2);
    visitor(MyEnum::val3, "val3", 3);
  }
};

template <typename Visitor>
struct Acceptor<MyEnum2, Visitor> {
  static constexpr void visit(Visitor&& visitor) {
    visitor(MyEnum2::val1, "val1", 0);
    visitor(MyEnum2::val2, "val2", 4);
  }
};

struct MyStruct {
  int v1;
  double v2;
};

struct MyStruct2 {
  int a;
  int b;
  MyStruct s;
};
// template <typename Visitor>
// struct Acceptor<MyStruct, Visitor> {
//   static constexpr void visit(Visitor&& visitor, MyStruct& myStruct) {
//     visitor(myStruct.v1);
//     visitor(myStruct.v2);
//   }
// };

template <typename Visitor>
constexpr void visit(MyStruct& myStruct, Visitor&& visitor) {
    visitor("v1", myStruct.v1);
    visitor("v2", myStruct.v2);
  }

template <typename Visitor>
constexpr void visit(MyStruct2& ms, Visitor&& visitor, bool visitNested = true) {
    visitor("a", ms.a);
    visitor("b", ms.b);
    visitor("s", ms.s);
    //visit(ms.s);
  }

template <typename Visitor>
struct Acceptor<MyStruct, Visitor> {
  static constexpr void visit(Visitor&& visitor) {
    visitor("v1");
    visitor("v2");
  }
};

template <typename Visitor>
struct Acceptor<MyStruct2, Visitor> {
  static constexpr void visit(Visitor&& visitor, bool visitNested=true) {
    visitor("a");
    visitor("b");
    if (visitNested) {
      Acceptor<decltype(MyStruct2::s), Visitor>::visit(std::forward<Visitor>(visitor));
    } else {
      visitor("s");
    }
    //visit<decltype(MyStruct2::s)>(std::forward<Visitor>(visitor));
  }
};

// helper to be able to automatically deduce VisitorType (must be specified for
// classes, CTAG doesn't apply.) Since this wrapper is now performing deduction,
// use universal references
template <typename EnumType, typename VisitorType>
constexpr void visit(VisitorType&& visitor) {
  Acceptor<EnumType, VisitorType>::visit(std::forward<VisitorType>(visitor));
}

// helper to be able to automatically deduce VisitorType
template <typename T, typename VisitorType>
constexpr void visit(VisitorType&& visitor, T&& t) {
  Acceptor<T, VisitorType>::visit(
      std::forward<VisitorType>(visitor), std::forward<T>(t));
}

//#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
// C++17 specific stuff here
//#endif
// C++17 supports constexpr lambdas
template <typename EnumType>
constexpr int enumCount2() {
  auto count = 0;
  auto l = [&count](const EnumType& e, const char* name,
                    std::underlying_type_t<EnumType> val) { count++; };
  visit<EnumType>(l);
  return count;
}

template <typename EnumType>
constexpr std::underlying_type_t<EnumType> toIndex(EnumType enumVal) {
  std::underlying_type_t<EnumType> index{};
  auto l = [enumVal, &index](const EnumType& e, const char* name,
                             std::underlying_type_t<EnumType> val) {
    if (e == enumVal) {
      index = val;
    }
  };
  visit<EnumType>(l);
  return index;
}

template <typename EnumType>
constexpr const char* toString(EnumType enumVal) {
  const char* s;
  auto l = [enumVal, &s](const EnumType& e, const char* name,
                         std::underlying_type_t<EnumType> val) {
    if (e == enumVal) {
      s = name;
    }
  };
  visit<EnumType>(l);
  return s;
}

template <typename T>
constexpr void increment(T&& t) {
  auto l = [](const char*, auto& field) { field++;};
  visit(std::forward<T>(t), l);
  //Acceptor<T, decltype(l)>::visit(l, t);
}


// C++14 does not support constexpr lambdas so use a functor instead
template <typename EnumType>
struct CountFunctor {
  int& _capturedCount;
  constexpr CountFunctor(int& capturedCount) : _capturedCount{capturedCount} {}

  constexpr void operator()(const EnumType& e, const char* name,
                            std::underlying_type_t<EnumType> val) {
    _capturedCount++;
  }
};

template <typename EnumType>
constexpr int enumCountFunctor2() {
  int count = 0;
  CountFunctor<EnumType> f{count};
  visit<EnumType>(f);
  return count;
}

template<typename T>
constexpr size_t field_count() {
  size_t count = 0;
  auto l = [&count](const char* n) {
    count++;
    std::cout << n <<"\n";
  };
  visit<T>(l);
  return count;
}

template<typename T>
constexpr size_t field_count(T&&) {
  return field_count<std::decay_t<T>>();
}

template<size_t i, typename T>
constexpr const char* get_name() {
  size_t count = 0;
  const char * out;
  auto l = [&count, &out](const char* name) constexpr {
    if (count == i) {
      out = name;
    }
    count++;
  };
  visit<T>(l);
  // extra layer of indirection required for integral constant expression
  //static_assert(field_count<T>() > i, "Index out of range!");
  return out;
}

template<size_t i, typename T>
constexpr const char* get_name(T&&) {
  return get_name<i, std::decay_t<T>>();
}

template<size_t i, typename T>
constexpr auto& get(const T& t) {
  size_t count = 0;
  // could use std::any in c++17? but what type to cast to?
  const void * out;
  auto l = [&count, &out](const char* name, auto& val) constexpr {
    if (count == i) {
      out = &val;
    }
    count++;
  };
  visit(t, l);
  // extra layer of indirection required for integral constant expression
  //static_assert(field_count<T>() > i, "Index out of range!");
  return *out;
}

constexpr bool strings_equal(char const * a, char const * b) {
    return *a == *b && (*a == '\0' || strings_equal(a + 1, b + 1));
}

int main() {
  static_assert(enumCount2<MyEnum>() == 3);
  std::cout << enumCount2<MyEnum>() << "\n";
  static_assert(enumCount2<MyEnum2>() == 2);
  std::cout << enumCount2<MyEnum2>() << "\n";
  static_assert(enumCountFunctor2<MyEnum2>() == 2);
  std::cout << enumCountFunctor2<MyEnum2>() << "\n";
  static_assert(toIndex(MyEnum::val3) == 3);
  std::cout << toIndex(MyEnum::val3) << "\n";
  static_assert(toIndex(MyEnum2::val2) == 4);
  std::cout << toIndex(MyEnum2::val2) << "\n";
  static_assert(strings_equal(toString(MyEnum2::val2), "val2"));
  std::cout << toString(MyEnum2::val2) << "\n";
  MyStruct s{3, 1.5};
  std::cout << s.v1 << " " << s.v2 << "\n";
  increment(s);
  std::cout << s.v1 << " " << s.v2 << "\n";
  //increment(MyStruct{3, 1.5});
  // or size
  std::cout << field_count(s) << "\n";
  std::cout << field_count( MyStruct{3, 1.5}) << "\n";
  //static_assert(field_count<MyStruct>() == 2);
  //static_assert(field_count( MyStruct{3, 1.5}) == 2);
  std::cout << field_count<MyStruct2>() << "\n";
  //visit_struct::visit_types<my_type>(v);
 //v("a", visit_struct::type_c<a>());

  //visit_struct::get<i>(s); // reference to i'th member of s

  static_assert(strings_equal(get_name<1, MyStruct>(), "v2"));
  static_assert(strings_equal(get_name<1>(MyStruct{}), "v2"));
  std::cout << get_name<1>(s) << "\n";
}
