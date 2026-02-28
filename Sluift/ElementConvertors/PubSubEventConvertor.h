/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEvent.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventConvertor : public GenericLuaElementConvertor<PubSubEvent> {
        public:
            PubSubEventConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventConvertor() override;

            virtual std::shared_ptr<PubSubEvent> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEvent>) override;

        private:
            LuaElementConvertors* convertors;
    };
}
