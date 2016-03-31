/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerConfigure.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerConfigureConvertor : public GenericLuaElementConvertor<PubSubOwnerConfigure> {
        public:
            PubSubOwnerConfigureConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerConfigureConvertor();

            virtual boost::shared_ptr<PubSubOwnerConfigure> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, boost::shared_ptr<PubSubOwnerConfigure>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
            LuaElementConvertors* convertors;
    };
}
