/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <boost/variant.hpp>

struct lua_State;

namespace Swift {
    namespace Lua {
        struct Nil {};

        typedef boost::make_recursive_variant<
                Nil,
                bool,
                int,
                std::string,
                std::vector< boost::recursive_variant_ >,
                std::map<std::string, std::shared_ptr<boost::recursive_variant_> >
            >::type Value;

        typedef std::map<std::string, std::shared_ptr<Value> > Table;
        typedef std::vector<Value> Array;

        inline std::shared_ptr<Value> nilRef() {
            return std::make_shared<Value>(Nil());
        }

        inline std::shared_ptr<Value> valueRef(const std::string& value) {
            return std::make_shared<Value>(value);
        }

        inline std::shared_ptr<Value> intRef(int value) {
            return std::make_shared<Value>(value);
        }

        inline std::shared_ptr<Value> boolRef(bool value) {
            return std::make_shared<Value>(value);
        }

        inline std::shared_ptr<Value> valueRef(const Table& table) {
            return std::make_shared<Value>(table);
        }

        inline std::shared_ptr<Value> valueRef(const Array& array) {
             return std::make_shared<Value>(array);
        }

        void pushValue(lua_State* state, const Value& value);
    }
}
