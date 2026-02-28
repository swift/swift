/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventRedirect.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventRedirectConvertor : public GenericLuaElementConvertor<PubSubEventRedirect> {
        public:
            PubSubEventRedirectConvertor();
            virtual ~PubSubEventRedirectConvertor() override;

            virtual std::shared_ptr<PubSubEventRedirect> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventRedirect>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
