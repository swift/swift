/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/MAMFin.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class MAMFinConvertor : public GenericLuaElementConvertor<MAMFin> {
        public:
            MAMFinConvertor(LuaElementConvertors* convertors);
            virtual ~MAMFinConvertor() override;

            virtual std::shared_ptr<MAMFin> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<MAMFin>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}

