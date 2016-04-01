/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Delay.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class DelayConvertor : public GenericLuaElementConvertor<Delay> {
        public:
            DelayConvertor();
            virtual ~DelayConvertor();

            virtual std::shared_ptr<Delay> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Delay>) SWIFTEN_OVERRIDE;
    };
}
