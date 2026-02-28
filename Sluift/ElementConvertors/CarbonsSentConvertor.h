/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/CarbonsSent.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class CarbonsSentConvertor : public GenericLuaElementConvertor<CarbonsSent> {
        public:
            CarbonsSentConvertor(LuaElementConvertors* convertors);
            virtual ~CarbonsSentConvertor() override;

            virtual std::shared_ptr<CarbonsSent> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<CarbonsSent>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}

