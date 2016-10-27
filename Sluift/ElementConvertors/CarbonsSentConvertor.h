/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/CarbonsSent.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class CarbonsSentConvertor : public GenericLuaElementConvertor<CarbonsSent> {
        public:
            CarbonsSentConvertor(LuaElementConvertors* convertors);
            virtual ~CarbonsSentConvertor();

            virtual std::shared_ptr<CarbonsSent> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<CarbonsSent>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
            LuaElementConvertors* convertors;
    };
}

