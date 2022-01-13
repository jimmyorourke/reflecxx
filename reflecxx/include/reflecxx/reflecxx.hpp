//            __ _
//  _ __ ___ / _| | ___  _____  ____  __
// | '__/ _ \ |_| |/ _ \/ __\ \/ /\ \/ /
// | | |  __/  _| |  __/ (__ >  <  >  <
// |_|  \___|_| |_|\___|\___/_/\_\/_/\_\
//
// Copyright (c) 2021-2022 Jimmy O'Rourke
// Licensed under and subject to the terms of the LICENSE file accompanying this distribution.
// Official repository: https://github.com/jimmyorourke/reflecxx

#pragma once

// Monolithic include header
// Note: does not include json_visitor.hpp as this library doesn't link/include nlohmann::json by default.

#include <reflecxx/attributes.hpp>
#include <reflecxx/enum_visitor.hpp>
#include <reflecxx/struct_visitor.hpp>
#include <reflecxx/visit.hpp>
