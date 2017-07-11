/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Status.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class StatusConvertor : public GenericLuaElementConvertor<Status> {
        public:
            StatusConvertor();
            virtual ~StatusConvertor() SWIFTEN_OVERRIDE;

            virtual std::shared_ptr<Status> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Status>) SWIFTEN_OVERRIDE;
    };
}
