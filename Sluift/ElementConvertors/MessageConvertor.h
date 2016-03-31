/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Message.h>

#include <Sluift/ElementConvertors/StanzaConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class MessageConvertor : public StanzaConvertor<Message> {
        public:
            MessageConvertor(LuaElementConvertors* convertors);
            virtual ~MessageConvertor();

            virtual boost::shared_ptr<Message> doConvertFromLua(lua_State*) SWIFTEN_OVERRIDE;
            virtual void doConvertToLua(lua_State*, boost::shared_ptr<Message>) SWIFTEN_OVERRIDE;

            virtual boost::optional<Documentation> getDocumentation() const SWIFTEN_OVERRIDE;

            static std::string convertMessageTypeToString(Message::Type type);
            static Message::Type convertMessageTypeFromString(const std::string& type);

        private:
            LuaElementConvertors* convertors;
    };
}

