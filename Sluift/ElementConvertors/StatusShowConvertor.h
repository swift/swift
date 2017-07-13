/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/StatusShow.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class StatusShowConvertor : public GenericLuaElementConvertor<StatusShow> {
        public:
            StatusShowConvertor();
            virtual ~StatusShowConvertor() override;

            virtual std::shared_ptr<StatusShow> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<StatusShow>) override;

            static std::string convertStatusShowTypeToString(const StatusShow::Type &show);
            static StatusShow::Type convertStatusShowTypeFromString(const std::string& show);
    };
}
