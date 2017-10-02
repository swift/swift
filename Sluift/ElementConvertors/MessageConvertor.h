/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Elements/Message.h>

#include <Sluift/ElementConvertors/StanzaConvertor.h>

namespace Swift {
    class LuaElementConvertors;

    class MessageConvertor : public StanzaConvertor<Message> {
        public:
            MessageConvertor(LuaElementConvertors* convertors);
            virtual ~MessageConvertor() override;

            virtual std::shared_ptr<Message> doConvertFromLua(lua_State*) override;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<Message>) override;

            virtual boost::optional<Documentation> getDocumentation() const override;

            static std::string convertMessageTypeToString(Message::Type type);
            static Message::Type convertMessageTypeFromString(const std::string& type);

        private:
            LuaElementConvertors* convertors;
    };
}

