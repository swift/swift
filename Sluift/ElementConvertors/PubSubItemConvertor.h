/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubItem.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubItemConvertor : public GenericLuaElementConvertor<PubSubItem> {
        public:
            PubSubItemConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubItemConvertor() override;

            virtual std::shared_ptr<PubSubItem> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubItem>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
