/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubAffiliation.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubAffiliationConvertor : public GenericLuaElementConvertor<PubSubAffiliation> {
        public:
            PubSubAffiliationConvertor();
            virtual ~PubSubAffiliationConvertor() override;

            virtual std::shared_ptr<PubSubAffiliation> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubAffiliation>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
