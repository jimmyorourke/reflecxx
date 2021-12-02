#include <gtest/gtest.h>

#include <proto/json_visitor.hpp>
#include <structs.hpp>

namespace {
test_types::NestingStruct buildNestingStruct() {
    test_types::BasicStruct b1{true, 1, 2.5};
    test_types::BasicStruct b2{false, -5, 3.4};
    return {9, -2.2, b1, {b1, b2, b1}, {b2, b2}};
}

nlohmann::json buildNestingStructJson() {
    // build json representation by hand
    return {{"i", 9},
            {"d", -2.2},
            {"bs", {{"b", true}, {"i", 1}, {"d", 2.5}}},
            {"basicsArr",
             {{{"b", true}, {"i", 1}, {"d", 2.5}},
              {{"b", false}, {"i", -5}, {"d", 3.4}},
              {{"b", true}, {"i", 1}, {"d", 2.5}}}},
            {"basicsStdarr", {{{"b", false}, {"i", -5}, {"d", 3.4}}, {{"b", false}, {"i", -5}, {"d", 3.4}}}}};
}
} // namespace

TEST(json_visitor, toJson) {
    test_types::NestingStruct s = buildNestingStruct();

    // to json
    nlohmann::json j = s;

    EXPECT_EQ(j.dump(), buildNestingStructJson().dump());
}

TEST(json_visitor, fromJson) {
    nlohmann::json j = buildNestingStructJson();

    // from json
    test_types::NestingStruct nsFromJson = j;

    EXPECT_EQ(nsFromJson, buildNestingStruct());
}
