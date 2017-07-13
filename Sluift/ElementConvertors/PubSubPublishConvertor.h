/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubPublish.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubPublishConvertor : public GenericLuaElementConvertor<PubSubPublish> {
        public:
            PubSubPublishConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubPublishConvertor() override;

            virtual std::shared_ptr<PubSubPublish> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubPublish>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
