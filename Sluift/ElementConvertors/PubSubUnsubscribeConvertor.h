/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubUnsubscribe.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubUnsubscribeConvertor : public GenericLuaElementConvertor<PubSubUnsubscribe> {
        public:
            PubSubUnsubscribeConvertor();
            virtual ~PubSubUnsubscribeConvertor() override;

            virtual std::shared_ptr<PubSubUnsubscribe> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubUnsubscribe>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
