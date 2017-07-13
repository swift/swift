/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/DiscoItems.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class DiscoItemsConvertor : public GenericLuaElementConvertor<DiscoItems> {
        public:
            DiscoItemsConvertor();
            virtual ~DiscoItemsConvertor() override;

            virtual std::shared_ptr<DiscoItems> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<DiscoItems>) override;
    };
}
