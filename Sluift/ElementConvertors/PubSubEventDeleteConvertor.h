/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventDelete.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventDeleteConvertor : public GenericLuaElementConvertor<PubSubEventDelete> {
        public:
            PubSubEventDeleteConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventDeleteConvertor() override;

            virtual std::shared_ptr<PubSubEventDelete> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventDelete>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
