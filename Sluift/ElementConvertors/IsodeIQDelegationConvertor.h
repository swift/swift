/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/IsodeIQDelegation.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class IsodeIQDelegationConvertor : public GenericLuaElementConvertor<IsodeIQDelegation> {
        public:
            IsodeIQDelegationConvertor(LuaElementConvertors* convertors);
            virtual ~IsodeIQDelegationConvertor() override;

            virtual std::shared_ptr<IsodeIQDelegation> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<IsodeIQDelegation>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
