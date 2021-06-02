#pragma once

#include <nlohmann/json.hpp>

// Requires that all members of T are themselves nlohmann::json serializable
// Requires 1:1 field mapping

namespace proto {

struct ToJsonVisitor {
    ToJsonVisitor(nlohmann::json& value)
    : jsonValue(value) {}
    nlohmann::json& jsonValue;

    template <typename T>
    void operator()(const char* name, const T& member) {
        jsonValue[name] = member;
    }
};

struct FromJsonVisitor {
    FromJsonVisitor(const nlohmann::json& value)
    : jsonValue(value) {}
    const nlohmann::json& jsonValue;

    template <typename T>
    void operator()(const char* name, T& member) const {
        fromJson(member, jsonValue.at(name));
    }

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
} // namespace proto

// The code below uses the generated visitor acceptors. To avoid problems if this header is included into headers that
// get compiled by the generator, don't define it during generation.
#ifndef PROTO_GENERATION

// Automatically create to/from nlohmann JSON functions for any proto visitable type.
// Note that since this uses the adl_serializer, if specialization for any type is desired it must also be done by
// specializing this adl_serializer struct rather than defining the to_json/from_json free functions (since ADL into the
// argument namespace will no longer apply).
namespace nlohmann {
template <typename T>
struct adl_serializer<T, std::enable_if_t<proto::is_proto_visitable_v<T>>> {
    static void to_json(json& j, const T& t) {
        proto::ToJsonVisitor v{j};
        proto::visit(t, std::move(v));
    }

    static void from_json(const json& j, T& t) {
        proto::FromJsonVisitor v{j};
        proto::visit(t, std::move(v));
    }
};
} // namespace nlohmann

#endif // PROTO_GENERATION
