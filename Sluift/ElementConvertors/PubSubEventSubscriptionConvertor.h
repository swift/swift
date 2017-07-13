/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventSubscription.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventSubscriptionConvertor : public GenericLuaElementConvertor<PubSubEventSubscription> {
        public:
            PubSubEventSubscriptionConvertor();
            virtual ~PubSubEventSubscriptionConvertor() override;

            virtual std::shared_ptr<PubSubEventSubscription> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventSubscription>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
