#pragma once

#include <proto/proto_base.hpp>

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef PROTO_GENERATION

namespace proto {

// Declares an alias to the type of the i'th visitable field of T.
template <size_t i, typename T>
using typeAt = typename std::tuple_element<i, TupleTypeT<T>>::type;

// Returns a reference to the i'th field in an instance of T.
template <size_t i, typename T>
constexpr auto& get(T& obj);

// Returns the number of fields in T.
template <typename T>
constexpr size_t fieldCount();

// Returns the name of the i'th field of T.
template <size_t i, typename T>
constexpr const char* getName();

// The variadic template args need to be last or type deduction doesn't work properly.
template <size_t, typename F, typename T, typename... Ts, typename = std::enable_if_t<(std::is_same_v<T, Ts> && ...)>>
constexpr void applyForEach(F&& f, T&& t1, Ts&&... ts);

template <typename T>
constexpr auto eql1(const T& t1, const T& t2);

} // namespace proto

#include "impl/struct_visitor_impl.hpp"

#endif // PROTO_GENERATION
