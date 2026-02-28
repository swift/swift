/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventConfiguration.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventConfigurationConvertor : public GenericLuaElementConvertor<PubSubEventConfiguration> {
        public:
            PubSubEventConfigurationConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventConfigurationConvertor() override;

            virtual std::shared_ptr<PubSubEventConfiguration> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventConfiguration>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
