/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerSubscriptions.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerSubscriptionsConvertor : public GenericLuaElementConvertor<PubSubOwnerSubscriptions> {
        public:
            PubSubOwnerSubscriptionsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerSubscriptionsConvertor();

            virtual boost::shared_ptr<PubSubOwnerSubscriptions> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubOwnerSubscriptions>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
            LuaElementConvertors* convertors;
    };
}
