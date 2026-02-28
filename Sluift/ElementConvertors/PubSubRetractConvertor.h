/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubRetract.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubRetractConvertor : public GenericLuaElementConvertor<PubSubRetract> {
        public:
            PubSubRetractConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubRetractConvertor() override;

            virtual std::shared_ptr<PubSubRetract> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubRetract>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
