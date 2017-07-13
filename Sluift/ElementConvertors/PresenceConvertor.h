/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Presence.h>

#include <Sluift/ElementConvertors/StanzaConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class PresenceConvertor : public StanzaConvertor<Presence> {
        public:
            PresenceConvertor(LuaElementConvertors* convertors);
            virtual ~PresenceConvertor() override;

            virtual std::shared_ptr<Presence> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Presence>) override;

            virtual boost::optional<Documentation> getDocumentation() const override;

            static std::string convertPresenceTypeToString(Presence::Type type);
            static Presence::Type convertPresenceTypeFromString(const std::string& type);

        private:
            LuaElementConvertors* convertors;
    };
}

