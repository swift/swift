/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Forwarded.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class ForwardedConvertor : public GenericLuaElementConvertor<Forwarded> {
        public:
            ForwardedConvertor(LuaElementConvertors* convertors);
            virtual ~ForwardedConvertor() override;

            virtual std::shared_ptr<Forwarded> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Forwarded>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}

