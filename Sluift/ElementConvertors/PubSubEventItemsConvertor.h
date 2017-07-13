/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventItems.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventItemsConvertor : public GenericLuaElementConvertor<PubSubEventItems> {
        public:
            PubSubEventItemsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventItemsConvertor() override;

            virtual std::shared_ptr<PubSubEventItems> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventItems>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
