/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubSubscribe.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubSubscribeConvertor : public GenericLuaElementConvertor<PubSubSubscribe> {
        public:
            PubSubSubscribeConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubSubscribeConvertor() override;

            virtual std::shared_ptr<PubSubSubscribe> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubSubscribe>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
