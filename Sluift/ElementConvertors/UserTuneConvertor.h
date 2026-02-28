/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/UserTune.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class UserTuneConvertor : public GenericLuaElementConvertor<UserTune> {
        public:
            UserTuneConvertor();
            virtual ~UserTuneConvertor() override;

            virtual std::shared_ptr<UserTune> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<UserTune>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
