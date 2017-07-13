/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/UserLocation.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class UserLocationConvertor : public GenericLuaElementConvertor<UserLocation> {
        public:
            UserLocationConvertor();
            virtual ~UserLocationConvertor() override;

            virtual std::shared_ptr<UserLocation> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<UserLocation>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
