/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Sluift/ElementConvertors/MessageConvertor.h>
#include <Sluift/LuaElementConvertors.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

MessageConvertor::MessageConvertor(LuaElementConvertors* convertors) : 
		StanzaConvertor("message"),
		convertors(convertors) {
}

MessageConvertor::~MessageConvertor() {
}

boost::shared_ptr<Message> MessageConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<Message> result = getStanza(L, convertors);
	lua_getfield(L, -1, "type");
	if (lua_isstring(L, -1)) {
		result->setType(convertMessageTypeFromString(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void MessageConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<Message> stanza) {
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
