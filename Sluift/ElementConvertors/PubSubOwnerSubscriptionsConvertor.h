/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerSubscriptions.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerSubscriptionsConvertor : public GenericLuaElementConvertor<PubSubOwnerSubscriptions> {
        public:
            PubSubOwnerSubscriptionsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerSubscriptionsConvertor() override;

            virtual std::shared_ptr<PubSubOwnerSubscriptions> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerSubscriptions>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
