/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Sluift/LuaElementConvertor.h>

namespace Swift {
    class DefaultElementConvertor : public LuaElementConvertor {
        public:
            DefaultElementConvertor();
            virtual ~DefaultElementConvertor() override;

            virtual std::shared_ptr<Element> convertFromLua(lua_State*, int index, const std::string& type) override;
            virtual boost::optional<std::string> convertToLua(lua_State*, std::shared_ptr<Element>) override;
    };
}
