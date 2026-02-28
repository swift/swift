/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/SecurityLabel.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class SecurityLabelConvertor : public GenericLuaElementConvertor<SecurityLabel> {
        public:
            SecurityLabelConvertor();
            virtual ~SecurityLabelConvertor() override;

            virtual std::shared_ptr<SecurityLabel> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<SecurityLabel>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;
    };
}
