/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerConfigure.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerConfigureConvertor : public GenericLuaElementConvertor<PubSubOwnerConfigure> {
        public:
            PubSubOwnerConfigureConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerConfigureConvertor() override;

            virtual std::shared_ptr<PubSubOwnerConfigure> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerConfigure>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
