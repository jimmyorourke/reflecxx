#include <gtest/gtest.h>

#include "test_types.hpp"

//#include <proto/proto_base.hpp>

#include "json_visitor.hpp"

// clean up
#include "../out.hpp"

namespace {
test_types::Wrapper buildDefaultWrapper() {
    test_types::Basics b1{true, 1, 2.5};
    test_types::Basics b2{false, -5, 3.4};
    return {9, -2.2, b1, {b1, b2, b1}, {b2, b2}};
}

nlohmann::json buildDefaultWrapperJson() {
    // build json representation by hand
    return {{"i", 9},
            {"d", -2.2},
            {"b", {{"b", true}, {"i", 1}, {"d", 2.5}}},
            {"basicsArr",
             {{{"b", true}, {"i", 1}, {"d", 2.5}},
              {{"b", false}, {"i", -5}, {"d", 3.4}},
              {{"b", true}, {"i", 1}, {"d", 2.5}}}},
            {"basicsStdarr", {{{"b", false}, {"i", -5}, {"d", 3.4}}, {{"b", false}, {"i", -5}, {"d", 3.4}}}}};
}
} // namespace

TEST(json_visitor, toJson) {
    test_types::Wrapper w = buildDefaultWrapper();

    // to json
    nlohmann::json j = w;

    EXPECT_EQ(j.dump(), buildDefaultWrapperJson().dump());
}

TEST(json_visitor, fromJson) {
    nlohmann::json wJson = buildDefaultWrapperJson();

    // from json
    test_types::Wrapper wFromJson = wJson;

    EXPECT_EQ(wFromJson, buildDefaultWrapper());
}
