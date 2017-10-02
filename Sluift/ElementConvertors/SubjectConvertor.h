/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Subject.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class SubjectConvertor : public GenericLuaElementConvertor<Subject> {
        public:
            SubjectConvertor();
            virtual ~SubjectConvertor() override;

            virtual std::shared_ptr<Subject> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Subject>) override;
    };
}
