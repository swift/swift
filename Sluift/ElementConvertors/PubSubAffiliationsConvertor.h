/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubAffiliations.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubAffiliationsConvertor : public GenericLuaElementConvertor<PubSubAffiliations> {
        public:
            PubSubAffiliationsConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubAffiliationsConvertor() override;

            virtual std::shared_ptr<PubSubAffiliations> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubAffiliations>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
