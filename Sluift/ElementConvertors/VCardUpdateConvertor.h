/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/VCardUpdate.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class VCardUpdateConvertor : public GenericLuaElementConvertor<VCardUpdate> {
        public:
            VCardUpdateConvertor();
            virtual ~VCardUpdateConvertor() override;

            virtual std::shared_ptr<VCardUpdate> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<VCardUpdate>) override;
    };
}
