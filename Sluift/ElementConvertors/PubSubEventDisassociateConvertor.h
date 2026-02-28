/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventDisassociate.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventDisassociateConvertor : public GenericLuaElementConvertor<PubSubEventDisassociate> {
        public:
            PubSubEventDisassociateConvertor();
            virtual ~PubSubEventDisassociateConvertor() override;

            virtual std::shared_ptr<PubSubEventDisassociate> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventDisassociate>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
