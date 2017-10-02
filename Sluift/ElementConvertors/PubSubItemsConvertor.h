/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubItems.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubItemsConvertor : public GenericLuaElementConvertor<PubSubItems> {
        public:
            PubSubItemsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubItemsConvertor() override;

            virtual std::shared_ptr<PubSubItems> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubItems>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
