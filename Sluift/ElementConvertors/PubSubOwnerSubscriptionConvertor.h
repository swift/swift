/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerSubscription.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerSubscriptionConvertor : public GenericLuaElementConvertor<PubSubOwnerSubscription> {
        public:
            PubSubOwnerSubscriptionConvertor();
            virtual ~PubSubOwnerSubscriptionConvertor() override;

            virtual std::shared_ptr<PubSubOwnerSubscription> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerSubscription>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
