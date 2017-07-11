/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/VCardUpdate.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class VCardUpdateConvertor : public GenericLuaElementConvertor<VCardUpdate> {
        public:
            VCardUpdateConvertor();
            virtual ~VCardUpdateConvertor() SWIFTEN_OVERRIDE;

            virtual std::shared_ptr<VCardUpdate> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<VCardUpdate>) SWIFTEN_OVERRIDE;
    };
}
