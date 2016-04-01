/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/StatusShow.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class StatusShowConvertor : public GenericLuaElementConvertor<StatusShow> {
        public:
            StatusShowConvertor();
            virtual ~StatusShowConvertor();

            virtual std::shared_ptr<StatusShow> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<StatusShow>) SWIFTEN_OVERRIDE;

            static std::string convertStatusShowTypeToString(const StatusShow::Type &show);
            static StatusShow::Type convertStatusShowTypeFromString(const std::string& show);
    };
}
