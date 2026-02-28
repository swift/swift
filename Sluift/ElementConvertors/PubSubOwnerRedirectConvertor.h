/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerRedirect.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerRedirectConvertor : public GenericLuaElementConvertor<PubSubOwnerRedirect> {
        public:
            PubSubOwnerRedirectConvertor();
            virtual ~PubSubOwnerRedirectConvertor() override;

            virtual std::shared_ptr<PubSubOwnerRedirect> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerRedirect>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
