/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubConfigure.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubConfigureConvertor : public GenericLuaElementConvertor<PubSubConfigure> {
        public:
            PubSubConfigureConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubConfigureConvertor() override;

            virtual std::shared_ptr<PubSubConfigure> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubConfigure>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
