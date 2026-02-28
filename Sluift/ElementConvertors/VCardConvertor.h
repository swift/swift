/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/VCard.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class VCardConvertor : public GenericLuaElementConvertor<VCard> {
        public:
            VCardConvertor();
            virtual ~VCardConvertor() override;

            virtual std::shared_ptr<VCard> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<VCard>) override;
    };
}
