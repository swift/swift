/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/VCard.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class VCardConvertor : public GenericLuaElementConvertor<VCard> {
        public:
            VCardConvertor();
            virtual ~VCardConvertor();

            virtual std::shared_ptr<VCard> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<VCard>) SWIFTEN_OVERRIDE;
    };
}
