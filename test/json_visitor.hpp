#pragma once
#ifndef NORTH_CODEGEN // If not inside code generator
#include <codegen/attributes.h>
#include <codegen/visitor.hpp>
#include <nlohmann/json.hpp>
#include <thalmic/ContextThrow.hpp>

// Provides nlohmann::json serialization/deserialization using generated_visitor
//
// Expected Use of this header:
// 1. Include some type T
// 2. Include generated_visitor for visitor for T
// 3. DEFINE_JSON(T) will generate nlohmann::json bindings for T automatically
// Requires that all members of T are themselves nlohmann::json serializable
// Current Version requires 1:1 field mapping, fields marked CODEGEN_REQUIRED will throw exceptions if they are not
// found, other fields will be skipped.

namespace njson {

/// Visitor for arbitrary JSON serialization
/// For use within DEFINE_JSON/DEFINE_TO_JSON
class ToJsonVisitor {
public:
    ToJsonVisitor(nlohmann::json& value)
    : jsonValue(value)
    {
    }
    nlohmann::json& jsonValue;

    template <typename T>
    void operator()(const T& value, std::string name, codegen::MetaData)
    {
        jsonValue[name] = value;
    }
};

/// MetaStruct visitor for arbitrary JSON deserialization
/// For use within DEFINE_JSON/DEFINE_FROM_JSON
class FromJsonVisitor {
public:
    FromJsonVisitor(const nlohmann::json& value)
    : jsonValue(value)
    {
    }
    const nlohmann::json& jsonValue;

    template <typename T>
    void operator()(T& value, std::string name, codegen::MetaData metadata) const
    {
        if (jsonValue.count(name)) {
            // .at() is verbose but has better error handling than =
            value = jsonValue.at(name).get<T>();
        } else if (metadata.attributes.count("required")) {
            CONTEXT_THROW(std::runtime_error, "Missing required field " << name);
        }
    }
};
} // namespace njson
