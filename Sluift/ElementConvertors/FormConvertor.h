/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Form.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class FormConvertor : public GenericLuaElementConvertor<Form> {
        public:
            FormConvertor();
            virtual ~FormConvertor();

            virtual std::shared_ptr<Form> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Form>) SWIFTEN_OVERRIDE;
    };
}
