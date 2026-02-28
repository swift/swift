/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Delay.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class DelayConvertor : public GenericLuaElementConvertor<Delay> {
        public:
            DelayConvertor();
            virtual ~DelayConvertor() override;

            virtual std::shared_ptr<Delay> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Delay>) override;
    };
}
