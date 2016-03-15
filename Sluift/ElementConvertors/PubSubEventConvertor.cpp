/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventConvertor.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubEventConvertor::PubSubEventConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEvent>("pubsub_event"),
		convertors(convertors) {
}

PubSubEventConvertor::~PubSubEventConvertor() {
}

boost::shared_ptr<PubSubEvent> PubSubEventConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEvent> result = boost::make_shared<PubSubEvent>();
	if (boost::shared_ptr<PubSubEventPayload> payload = boost::dynamic_pointer_cast<PubSubEventPayload>(convertors->convertFromLua(L, -1))) {
		result->setPayload(payload);
	}
	return result;
}

void PubSubEventConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEvent> event) {
	convertors->convertToLua(L,  event->getPayload());
}
