/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubSubscriptions.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubSubscriptionsConvertor : public GenericLuaElementConvertor<PubSubSubscriptions> {
        public:
            PubSubSubscriptionsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubSubscriptionsConvertor() override;

            virtual std::shared_ptr<PubSubSubscriptions> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubSubscriptions>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
