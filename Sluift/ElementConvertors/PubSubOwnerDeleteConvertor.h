/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerDelete.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerDeleteConvertor : public GenericLuaElementConvertor<PubSubOwnerDelete> {
        public:
            PubSubOwnerDeleteConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerDeleteConvertor() override;

            virtual std::shared_ptr<PubSubOwnerDelete> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerDelete>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
