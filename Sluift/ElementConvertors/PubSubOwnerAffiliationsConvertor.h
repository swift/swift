/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubOwnerAffiliations.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubOwnerAffiliationsConvertor : public GenericLuaElementConvertor<PubSubOwnerAffiliations> {
        public:
            PubSubOwnerAffiliationsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubOwnerAffiliationsConvertor() override;

            virtual std::shared_ptr<PubSubOwnerAffiliations> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubOwnerAffiliations>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
