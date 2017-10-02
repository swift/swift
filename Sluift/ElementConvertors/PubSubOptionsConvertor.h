/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOptions.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOptionsConvertor : public GenericLuaElementConvertor<PubSubOptions> {
        public:
            PubSubOptionsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOptionsConvertor() override;

            virtual std::shared_ptr<PubSubOptions> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOptions>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
