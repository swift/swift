/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Sluift/ElementConvertors/PubSubAffiliationConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubAffiliationConvertor::PubSubAffiliationConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubAffiliation>("pubsub_affiliation"),
		convertors(convertors) {
}

PubSubAffiliationConvertor::~PubSubAffiliationConvertor() {
}

boost::shared_ptr<PubSubAffiliation> PubSubAffiliationConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubAffiliation> result = boost::make_shared<PubSubAffiliation>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "type");
	if (lua_isstring(L, -1)) {
		if (std::string(lua_tostring(L, -1)) == "none") {
			result->setType(PubSubAffiliation::None);
		}
		if (std::string(lua_tostring(L, -1)) == "member") {
			result->setType(PubSubAffiliation::Member);
		}
		if (std::string(lua_tostring(L, -1)) == "outcast") {
			result->setType(PubSubAffiliation::Outcast);
		}
		if (std::string(lua_tostring(L, -1)) == "owner") {
			result->setType(PubSubAffiliation::Owner);
		}
		if (std::string(lua_tostring(L, -1)) == "publisher") {
			result->setType(PubSubAffiliation::Publisher);
		}
		if (std::string(lua_tostring(L, -1)) == "publish_only") {
			result->setType(PubSubAffiliation::PublishOnly);
		}
	}
	lua_pop(L, 1);
	return result;
}

void PubSubAffiliationConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubAffiliation> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
	switch (payload->getType()) {
		case PubSubAffiliation::None:
			lua_pushstring(L, "none");
			break;
		case PubSubAffiliation::Member:
			lua_pushstring(L, "member");
			break;
		case PubSubAffiliation::Outcast:
			lua_pushstring(L, "outcast");
			break;
		case PubSubAffiliation::Owner:
			lua_pushstring(L, "owner");
			break;
		case PubSubAffiliation::Publisher:
			lua_pushstring(L, "publisher");
			break;
		case PubSubAffiliation::PublishOnly:
			lua_pushstring(L, "publish_only");
			break;
	}
	lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> PubSubAffiliationConvertor::getDocumentation() const {
	return Documentation(
		"PubSubAffiliation",
		"This table has the following fields:\n\n"
		"- `node`: string\n"
		"- `type`: `\"none\"`, `\"member\"`, `\"outcast\"`, `\"owner\"`, `\"publisher\"`, or `\"publish_only\"`\n"
	);
}
