/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerDefault.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerDefaultConvertor : public GenericLuaElementConvertor<PubSubOwnerDefault> {
        public:
            PubSubOwnerDefaultConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerDefaultConvertor() override;

            virtual std::shared_ptr<PubSubOwnerDefault> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerDefault>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
