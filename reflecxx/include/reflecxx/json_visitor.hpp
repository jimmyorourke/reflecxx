// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef REFLECXX_GENERATION

#include <reflecxx/visit.hpp>

// Note: This library does not link against/set include dirs for nlohmann json by default!
#include <nlohmann/json.hpp>

#include <string>

// Automatically define to/from nlohmann JSON functions for any reflecxx visitable type. Wow!
// Note that since this uses the adl_serializer, if specialization for any type is desired it must also be done by
// specializing this adl_serializer struct rather than defining the to_json/from_json free functions (since ADL into the
// argument namespace will no longer apply).
namespace nlohmann {
template <typename T>
struct adl_serializer<T, std::enable_if_t<reflecxx::is_reflecxx_visitable_v<T>>> {
    static void to_json(json& j, const T& t) {
        reflecxx::ToJsonVisitor v{j};
        reflecxx::visit(t, std::move(v));
    }

    static void from_json(const json& j, T& t) {
        reflecxx::FromJsonVisitor v{j};
        reflecxx::visit(t, std::move(v));
    }
};
} // namespace nlohmann

namespace reflecxx {

// Visitor functor for converting a named value (such as a class member) to JSON.
// Requires that T is nlohmann::json serializable (possibly through the use of this visitor on T's fundamental type
// members)..
struct ToJsonVisitor {
    ToJsonVisitor(nlohmann::json& value)
    : jsonValue(value) {}

    template <typename T>
    void operator()(std::string_view name, const T& member) {
        // std::string required, https://github.com/nlohmann/json/issues/1529
        jsonValue[std::string{name}] = member;
    }

    nlohmann::json& jsonValue;
};

// Visitor functor for converting from JSON to a named object (such as a class member).
struct FromJsonVisitor {
    FromJsonVisitor(const nlohmann::json& value)
    : jsonValue(value) {}

    template <typename T>
    void operator()(std::string_view name, T& member) const {
        // std::string required, https://github.com/nlohmann/json/issues/1529
        fromJson(member, jsonValue.at(std::string{name}));
    }

    const nlohmann::json& jsonValue;

 private:
    template <typename T>
    void fromJson(T& value, const nlohmann::json& jsonObj) const {
        value = jsonObj.get<T>();
    }
    // nlohmann::json.get() doesn't handle c-style arrays
    template <typename T, size_t N>
    void fromJson(T (&arr)[N], const nlohmann::json& jsonObj) const {
        if (jsonObj.size() != N) {
            throw std::runtime_error("JSON array size is different than expected");
        }
        size_t index = 0;
        for (auto& item : jsonObj) {
            arr[index++] = item.get<T>();
        }
    }
};

} // namespace reflecxx

#endif // REFLECXX_GENERATION
