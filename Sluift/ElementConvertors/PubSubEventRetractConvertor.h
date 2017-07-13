/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventRetract.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventRetractConvertor : public GenericLuaElementConvertor<PubSubEventRetract> {
        public:
            PubSubEventRetractConvertor();
            virtual ~PubSubEventRetractConvertor() override;

            virtual std::shared_ptr<PubSubEventRetract> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventRetract>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
