/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerAffiliation.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerAffiliationConvertor : public GenericLuaElementConvertor<PubSubOwnerAffiliation> {
        public:
            PubSubOwnerAffiliationConvertor();
            virtual ~PubSubOwnerAffiliationConvertor() override;

            virtual std::shared_ptr<PubSubOwnerAffiliation> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerAffiliation>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
