/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/CarbonsReceived.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class CarbonsReceivedConvertor : public GenericLuaElementConvertor<CarbonsReceived> {
        public:
            CarbonsReceivedConvertor(LuaElementConvertors* convertors);
            virtual ~CarbonsReceivedConvertor();

            virtual std::shared_ptr<CarbonsReceived> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<CarbonsReceived>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
            LuaElementConvertors* convertors;
    };
}

