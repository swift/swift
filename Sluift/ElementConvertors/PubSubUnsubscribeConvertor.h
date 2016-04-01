/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubUnsubscribe.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubUnsubscribeConvertor : public GenericLuaElementConvertor<PubSubUnsubscribe> {
        public:
            PubSubUnsubscribeConvertor();
            virtual ~PubSubUnsubscribeConvertor();

            virtual std::shared_ptr<PubSubUnsubscribe> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubUnsubscribe>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;
    };
}
