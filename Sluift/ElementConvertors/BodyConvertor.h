/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Body.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class BodyConvertor : public GenericLuaElementConvertor<Body> {
        public:
            BodyConvertor();
            virtual ~BodyConvertor();

            virtual std::shared_ptr<Body> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Body>) SWIFTEN_OVERRIDE;
    };
}
