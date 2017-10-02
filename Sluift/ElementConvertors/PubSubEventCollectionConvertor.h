/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/PubSubEventCollection.h>

#include <Sluift/GenericLuaElementConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PubSubEventCollectionConvertor : public GenericLuaElementConvertor<PubSubEventCollection> {
        public:
            PubSubEventCollectionConvertor(LuaElementConvertors* convertors);
            virtual ~PubSubEventCollectionConvertor() override;

            virtual std::shared_ptr<PubSubEventCollection> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<PubSubEventCollection>) override;
            virtual boost::optional<Documentation> getDocumentation() const override;

        private:
            LuaElementConvertors* convertors;
    };
}
