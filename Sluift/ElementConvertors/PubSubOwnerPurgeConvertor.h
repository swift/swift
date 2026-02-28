/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerPurge.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerPurgeConvertor : public GenericLuaElementConvertor<PubSubOwnerPurge> {
        public:
            PubSubOwnerPurgeConvertor();
            virtual ~PubSubOwnerPurgeConvertor() override;

            virtual std::shared_ptr<PubSubOwnerPurge> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerPurge>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
