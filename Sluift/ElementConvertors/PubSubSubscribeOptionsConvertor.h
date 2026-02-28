/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubSubscribeOptions.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubSubscribeOptionsConvertor : public GenericLuaElementConvertor<PubSubSubscribeOptions> {
        public:
            PubSubSubscribeOptionsConvertor();
            virtual ~PubSubSubscribeOptionsConvertor() override;

            virtual std::shared_ptr<PubSubSubscribeOptions> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubSubscribeOptions>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
