/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventAssociate.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventAssociateConvertor : public GenericLuaElementConvertor<PubSubEventAssociate> {
        public:
            PubSubEventAssociateConvertor();
            virtual ~PubSubEventAssociateConvertor() override;

            virtual std::shared_ptr<PubSubEventAssociate> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventAssociate>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
