/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <boost/optional.hpp>

struct lua_State;

namespace Swift {
    class Element;

    class LuaElementConvertor {
        public:
            static boost::optional<std::string> NO_RESULT;

            struct Documentation {
                Documentation(const std::string& className, const std::string& description) :
                    className(className), description(description) {}
                std::string className;
                std::string description;
            };

            virtual ~LuaElementConvertor();

            virtual std::shared_ptr<Element> convertFromLua(lua_State*, int index, const std::string& type) = 0;
            virtual boost::optional<std::string> convertToLua(lua_State*, std::shared_ptr<Element>) = 0;

            virtual boost::optional<Documentation> getDocumentation() const {
                return boost::optional<Documentation>();
            }
    };
}
