/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Sluift/ElementConvertors/IQConvertor.h>
#include <Sluift/LuaElementConvertors.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

IQConvertor::IQConvertor(LuaElementConvertors* convertors) : 
		StanzaConvertor("iq"),
		convertors(convertors) {
}

IQConvertor::~IQConvertor() {
}

boost::shared_ptr<IQ> IQConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<IQ> result = getStanza(L, convertors);
	lua_getfield(L, -1, "type");
	if (lua_isstring(L, -1)) {
		result->setType(IQConvertor::convertIQTypeFromString(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void IQConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<IQ> stanza) {
	pushStanza(L, stanza, convertors);
	const std::string type = IQConvertor::convertIQTypeToString(stanza->getType());
	lua_pushstring(L, type.c_str());
	lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> IQConvertor::getDocumentation() const {
	return Documentation(
		"IQ",
		"This table has the following fields:\n\n"
		"- `type`: string\n"
		"- `id`: string\n"
		"- `from`: string\n"
		"- `to`: string\n"
		"- `payloads`: array<@{Payload}>\n"
	);
}

std::string IQConvertor::convertIQTypeToString(IQ::Type type) {
	switch (type) {
		case IQ::Get: return "get";
		case IQ::Set: return "set";
		case IQ::Result: return "result";
		case IQ::Error: return "error";
	}
	assert(false);
	return "";
}

IQ::Type IQConvertor::convertIQTypeFromString(const std::string& type) {
	if (type == "get") {
		return IQ::Get;
	}
	else if (type == "set") {
		return IQ::Set;
	}
	else {
		throw Lua::Exception("Illegal query type: '" + type + "'");
	}
}
