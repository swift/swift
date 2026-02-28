/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/MAMResult.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class MAMResultConvertor : public GenericLuaElementConvertor<MAMResult> {
        public:
            MAMResultConvertor(LuaElementConvertors* convertors);
            virtual ~MAMResultConvertor() override;

            virtual std::shared_ptr<MAMResult> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<MAMResult>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}

