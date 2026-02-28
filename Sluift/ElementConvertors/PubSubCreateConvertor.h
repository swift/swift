/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubCreate.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubCreateConvertor : public GenericLuaElementConvertor<PubSubCreate> {
        public:
            PubSubCreateConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubCreateConvertor() override;

            virtual std::shared_ptr<PubSubCreate> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubCreate>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
