/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubOwnerAffiliations.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerAffiliationsConvertor : public GenericLuaElementConvertor<PubSubOwnerAffiliations> {
        public:
            PubSubOwnerAffiliationsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerAffiliationsConvertor();

            virtual std::shared_ptr<PubSubOwnerAffiliations> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerAffiliations>) SWIFTEN_OVERRIDE;
            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

        private:
            LuaElementConvertors* convertors;
    };
}
