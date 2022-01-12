#pragma once

// Autogenerated at 2022-01-12 00:39:53.102745 by visitor_generator.py.
// Do not edit, changes will be overwritten!

#include <type_traits>

#include <reflecxx/reflecxx_base.hpp>

namespace reflecxx {

////////////////////////////////////////////////////////////
// test_types::BasicClass
////////////////////////////////////////////////////////////

template <>
struct MetaStructInternal<test_types::BasicClass> {
    using Type = test_types::BasicClass;
    static constexpr auto publicFields = std::make_tuple(
        ClassMember<Type, bool>{&Type::b, "b"},
        ClassMember<Type, int>{&Type::i, "i"},
        ClassMember<Type, double>{&Type::d, "d"}
    );
    static constexpr auto baseClasses = std::make_tuple(
    );
};

////////////////////////////////////////////////////////////
// test_types::ChildClass
////////////////////////////////////////////////////////////

template <>
struct MetaStructInternal<test_types::ChildClass> {
    using Type = test_types::ChildClass;
    static constexpr auto publicFields = std::make_tuple(
        ClassMember<Type, int>{&Type::publicField, "publicField"}
    );
    static constexpr auto baseClasses = std::make_tuple(
        type_tag<test_types::BasicClass>{}
    );
};

////////////////////////////////////////////////////////////
// test_types::SecondLevelChildClass
////////////////////////////////////////////////////////////

template <>
struct MetaStructInternal<test_types::SecondLevelChildClass> {
    using Type = test_types::SecondLevelChildClass;
    static constexpr auto publicFields = std::make_tuple(
        ClassMember<Type, double>{&Type::someField, "someField"}
    );
    static constexpr auto baseClasses = std::make_tuple(
        type_tag<test_types::ChildClass>{}
    );
};

////////////////////////////////////////////////////////////
// test_types::ChildOfUnreflectedBaseClass
////////////////////////////////////////////////////////////

template <>
struct MetaStructInternal<test_types::ChildOfUnreflectedBaseClass> {
    using Type = test_types::ChildOfUnreflectedBaseClass;
    static constexpr auto publicFields = std::make_tuple(
        ClassMember<Type, int>{&Type::childField, "childField"}
    );
    static constexpr auto baseClasses = std::make_tuple(
        // skipping unannotated base class test_types::UnreflectedBaseClass
    );
};

} // namespace reflecxx
