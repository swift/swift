/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/MAMQuery.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class MAMQueryConvertor : public GenericLuaElementConvertor<MAMQuery> {
        public:
            MAMQueryConvertor(LuaElementConvertors* convertors);
            virtual ~MAMQueryConvertor() override;

            virtual std::shared_ptr<MAMQuery> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<MAMQuery>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}

