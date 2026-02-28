/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventPurge.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventPurgeConvertor : public GenericLuaElementConvertor<PubSubEventPurge> {
        public:
            PubSubEventPurgeConvertor();
            virtual ~PubSubEventPurgeConvertor() override;

            virtual std::shared_ptr<PubSubEventPurge> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventPurge>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
