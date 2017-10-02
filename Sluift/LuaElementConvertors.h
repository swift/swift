/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <boost/optional.hpp>

struct lua_State;

namespace Swift {
    class LuaElementConvertor;
    class Element;

    class LuaElementConvertors {
        public:
            LuaElementConvertors();
            virtual ~LuaElementConvertors();

            std::shared_ptr<Element> convertFromLua(lua_State*, int index);
            int convertToLua(lua_State*, std::shared_ptr<Element>);

            /**
             * Adds a toplevel type+data table with the given type.
             */
            std::shared_ptr<Element> convertFromLuaUntyped(lua_State*, int index, const std::string& type);

            /**
             * Strips the toplevel type+data table, and only return the
             * data.
             */
            int convertToLuaUntyped(lua_State*, std::shared_ptr<Element>);

            const std::vector< std::shared_ptr<LuaElementConvertor> >& getConvertors() const {
                return convertors;
            }

        private:
            boost::optional<std::string> doConvertToLuaUntyped(lua_State*, std::shared_ptr<Element>);
            void registerConvertors();

        private:
            std::vector< std::shared_ptr<LuaElementConvertor> > convertors;
    };
}
