/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventItem.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventItemConvertor : public GenericLuaElementConvertor<PubSubEventItem> {
        public:
            PubSubEventItemConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventItemConvertor() override;

            virtual std::shared_ptr<PubSubEventItem> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventItem>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
