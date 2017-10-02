/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/CarbonsReceived.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class CarbonsReceivedConvertor : public GenericLuaElementConvertor<CarbonsReceived> {
        public:
            CarbonsReceivedConvertor(LuaElementConvertors* convertors);
            virtual ~CarbonsReceivedConvertor() override;

            virtual std::shared_ptr<CarbonsReceived> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<CarbonsReceived>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}

