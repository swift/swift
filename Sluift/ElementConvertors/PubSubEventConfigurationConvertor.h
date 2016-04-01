/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubEventConfiguration.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventConfigurationConvertor : public GenericLuaElementConvertor<PubSubEventConfiguration> {
        public:
            PubSubEventConfigurationConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventConfigurationConvertor();

            virtual std::shared_ptr<PubSubEventConfiguration> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventConfiguration>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
            LuaElementConvertors* convertors;
    };
}
