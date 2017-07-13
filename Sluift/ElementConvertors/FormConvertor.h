/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Form.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class FormConvertor : public GenericLuaElementConvertor<Form> {
        public:
            FormConvertor();
            virtual ~FormConvertor() override;

            virtual std::shared_ptr<Form> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Form>) override;
    };
}
