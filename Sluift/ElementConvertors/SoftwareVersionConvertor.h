/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/SoftwareVersion.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class SoftwareVersionConvertor : public GenericLuaElementConvertor<SoftwareVersion> {
        public:
            SoftwareVersionConvertor();
            virtual ~SoftwareVersionConvertor() override;

            virtual std::shared_ptr<SoftwareVersion> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<SoftwareVersion>) override;
    };
}
