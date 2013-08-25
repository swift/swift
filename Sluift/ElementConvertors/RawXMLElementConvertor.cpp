/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/RawXMLElementConvertor.h>

#include <iostream>
#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>

#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Sluift/Lua/Check.h>

using namespace Swift;

RawXMLElementConvertor::RawXMLElementConvertor() {
}

RawXMLElementConvertor::~RawXMLElementConvertor() {
}

boost::shared_ptr<Payload> RawXMLElementConvertor::convertFromLua(lua_State* L, int index, const std::string& type) {
	if (type == "xml") {
		return boost::make_shared<RawXMLPayload>(std::string(Lua::checkString(L, index)));
	}
	return boost::shared_ptr<Payload>();
}

boost::optional<std::string> RawXMLElementConvertor::convertToLua(lua_State* L, boost::shared_ptr<Payload> payload) {
	PayloadSerializer* serializer = serializers.getPayloadSerializer(payload);
	assert(serializer);
	lua_pushstring(L, serializer->serialize(payload).c_str());
	return std::string("xml");
}
