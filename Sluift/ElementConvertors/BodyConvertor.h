/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Body.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class BodyConvertor : public GenericLuaElementConvertor<Body> {
        public:
            BodyConvertor();
            virtual ~BodyConvertor() override;

            virtual std::shared_ptr<Body> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Body>) override;
    };
}
