/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/MessageConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

MessageConvertor::MessageConvertor(LuaElementConvertors* convertors) :
        StanzaConvertor<Message>("message"),
        convertors(convertors) {
}

MessageConvertor::~MessageConvertor() {
}

std::shared_ptr<Message> MessageConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<Message> result = getStanza(L, convertors);
    lua_getfield(L, -1, "type");
    if (lua_isstring(L, -1)) {
        result->setType(convertMessageTypeFromString(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void MessageConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Message> stanza) {
    pushStanza(L, stanza, convertors);
    const std::string type = convertMessageTypeToString(stanza->getType());
    lua_pushstring(L, type.c_str());
    lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> MessageConvertor::getDocumentation() const {
    return Documentation(
        "Message",
        "This table has the following fields:\n\n"
        "- `type`: string\n"
        "- `id`: string\n"
        "- `from`: string\n"
        "- `to`: string\n"
        "- `payloads`: array<@{Payload}>\n"
    );
}

std::string MessageConvertor::convertMessageTypeToString(Message::Type type) {
    switch (type) {
        case Message::Normal: return "normal";
        case Message::Chat: return "chat";
        case Message::Error: return "error";
        case Message::Groupchat: return "groupchat";
        case Message::Headline: return "headline";
    }
    assert(false);
    return "";
}

Message::Type MessageConvertor::convertMessageTypeFromString(const std::string& type) {
    if (type == "normal") {
        return Message::Normal;
    }
    else if (type == "chat") {
        return Message::Chat;
    }
    else if (type == "error") {
        return Message::Error;
    }
    else if (type == "groupchat") {
        return Message::Groupchat;
    }
    else if (type == "headline") {
        return Message::Headline;
    }
    else {
        throw Lua::Exception("Illegal message type: '" + type + "'");
    }
}
