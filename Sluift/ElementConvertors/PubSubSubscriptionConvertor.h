/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubSubscription.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubSubscriptionConvertor : public GenericLuaElementConvertor<PubSubSubscription> {
        public:
            PubSubSubscriptionConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubSubscriptionConvertor() override;

            virtual std::shared_ptr<PubSubSubscription> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubSubscription>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
