/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Forwarded.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class ForwardedConvertor : public GenericLuaElementConvertor<Forwarded> {
        public:
            ForwardedConvertor(LuaElementConvertors* convertors);
            virtual ~ForwardedConvertor();

            virtual std::shared_ptr<Forwarded> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Forwarded>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
        private:
            LuaElementConvertors* convertors;
    };
}

