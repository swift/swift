/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/DiscoInfo.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class DiscoInfoConvertor : public GenericLuaElementConvertor<DiscoInfo> {
        public:
            DiscoInfoConvertor(LuaElementConvertors* convertors);
            virtual ~DiscoInfoConvertor() override;

            virtual std::shared_ptr<DiscoInfo> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<DiscoInfo>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
