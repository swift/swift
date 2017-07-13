/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubDefault.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubDefaultConvertor : public GenericLuaElementConvertor<PubSubDefault> {
        public:
            PubSubDefaultConvertor();
            virtual ~PubSubDefaultConvertor() override;

            virtual std::shared_ptr<PubSubDefault> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubDefault>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
