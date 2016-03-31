/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Subject.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class SubjectConvertor : public GenericLuaElementConvertor<Subject> {
        public:
            SubjectConvertor();
            virtual ~SubjectConvertor();

            virtual boost::shared_ptr<Subject> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, boost::shared_ptr<Subject>) SWIFTEN_OVERRIDE;
    };
}
