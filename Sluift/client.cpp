/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/assign/list_of.hpp>
#include <iostream>

#include <Sluift/SluiftClient.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Roster/SetRosterRequest.h>
#include <Swiften/Presence/SubscriptionManager.h>
#include <Swiften/Roster/XMPPRosterItem.h>
#include <Sluift/Watchdog.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>
#include <Sluift/Lua/FunctionRegistration.h>
#include <Swiften/Base/foreach.h>
#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/Value.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/globals.h>

using namespace Swift;
namespace lambda = boost::lambda;

static const std::string SLUIFT_CLIENT = Lua::FunctionRegistry::getMetaTableNameForType("Client");

static inline SluiftClient* getClient(lua_State* L) {
	return *Lua::checkUserData<SluiftClient>(L, 1, SLUIFT_CLIENT.c_str());
}

SLUIFT_LUA_FUNCTION(Client, async_connect) {
	SluiftClient* client = getClient(L);

	std::string host;
	if (lua_istable(L, 2)) {
		if (boost::optional<std::string> hostString = Lua::getStringField(L, 2, "host")) {
			host = *hostString;
		}
	}
	if (host.empty()) {
		client->connect();
	}
	else {
		client->connect(host);
	}
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, wait_connected) {
	getClient(L)->waitConnected();
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, is_connected) {
	lua_pushboolean(L, getClient(L)->isConnected());
	return 1;
}

SLUIFT_LUA_FUNCTION(Client, disconnect) {
	getClient(L)->disconnect();
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, set_version) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	if (boost::shared_ptr<SoftwareVersion> version = boost::dynamic_pointer_cast<SoftwareVersion>(Sluift::globals.elementConvertor.convertFromLuaUntyped(L, 2, "software_version"))) {
		client->setSoftwareVersion(version->getName(), version->getVersion(), version->getOS());
	}
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, get_contacts) {
	Sluift::globals.eventLoop.runOnce();

	SluiftClient* client = getClient(L);
	Lua::Table contactsTable;
	foreach(const XMPPRosterItem& item, client->getRoster()) {
		std::string subscription;
		switch(item.getSubscription()) {
			case RosterItemPayload::None: subscription = "none"; break;
			case RosterItemPayload::To: subscription = "to"; break;
			case RosterItemPayload::From: subscription = "from"; break;
			case RosterItemPayload::Both: subscription = "both"; break;
			case RosterItemPayload::Remove: subscription = "remove"; break;
		}
		Lua::Table itemTable = boost::assign::map_list_of
			("jid", boost::make_shared<Lua::Value>(item.getJID().toString()))
			("name", boost::make_shared<Lua::Value>(item.getName()))
			("subscription", boost::make_shared<Lua::Value>(subscription))
			("groups", boost::make_shared<Lua::Value>(std::vector<Lua::Value>(item.getGroups().begin(), item.getGroups().end())));
		contactsTable[item.getJID().toString()] = boost::make_shared<Lua::Value>(itemTable);
	}
	pushValue(L, contactsTable);
	Lua::registerTableToString(L, -1);
	return 1;
}

SLUIFT_LUA_FUNCTION(Client, send_message) {
	Sluift::globals.eventLoop.runOnce();
	JID to;
	std::string body;
	int index = 2;
	Message::Type type = Message::Chat;
	if (lua_isstring(L, index)) {
		to = std::string(lua_tostring(L, index));
		++index;
		if (!lua_isstring(L, index)) {
			std::string body = lua_tostring(L, index);
			++index;
		}
	}
	if (lua_istable(L, index)) {
		if (boost::optional<std::string> value = Lua::getStringField(L, index, "to")) {
			to = *value;
		}

		if (boost::optional<std::string> value = Lua::getStringField(L, index, "body")) {
			body = *value;
		}

		if (boost::optional<std::string> value = Lua::getStringField(L, index, "type")) {
			if (*value == "normal") {
				type = Message::Normal;
			}
			else if (*value == "chat") {
				type = Message::Chat;
			}
			else if (*value == "error") {
				type = Message::Error;
			}
			else if (*value == "groupchat") {
				type = Message::Groupchat;
			}
			else if (*value == "headline") {
				type = Message::Headline;
			}
		}
	}

	if (!to.isValid()) {
		throw Lua::Exception("Missing 'to'");
	}
	if (body.empty()) {
		throw Lua::Exception("Missing 'body'");
	}

	Message::ref message = boost::make_shared<Message>();
	message->setTo(to);
	message->setBody(body);
	message->setType(type);
	getClient(L)->getClient()->sendMessage(message);
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, send_presence) {
	Sluift::globals.eventLoop.runOnce();
	boost::shared_ptr<Presence> presence = boost::make_shared<Presence>();
	
	int index = 2;
	if (lua_isstring(L, index)) {
		presence->setStatus(lua_tostring(L, index));
		++index;
	}
	if (lua_istable(L, index)) {
		if (boost::optional<std::string> value = Lua::getStringField(L, index, "to")) {
			presence->setTo(*value);
		}
		if (boost::optional<std::string> value = Lua::getStringField(L, index, "status")) {
			presence->setStatus(*value);
		}
		if (boost::optional<int> value = Lua::getIntField(L, index, "priority")) {
			presence->setPriority(*value);
		}
		if (boost::optional<std::string> value = Lua::getStringField(L, index, "type")) {
			if (*value == "available") {
				presence->setType(Presence::Available);
			}
			else if (*value == "error") {
				presence->setType(Presence::Error);
			}
			else if (*value == "probe") {
				presence->setType(Presence::Probe);
			}
			else if (*value == "subscribe") {
				presence->setType(Presence::Subscribe);
			}
			else if (*value == "subscribed") {
				presence->setType(Presence::Subscribed);
			}
			else if (*value == "unavailable") {
				presence->setType(Presence::Unavailable);
			}
			else if (*value == "unsubscribe") {
				presence->setType(Presence::Unsubscribe);
			}
			else if (*value == "unsubscribed") {
				presence->setType(Presence::Unsubscribed);
			}
		}
	}

	getClient(L)->getClient()->getPresenceSender()->sendPresence(presence);
	lua_pushvalue(L, 1);
	return 0;
}

static int sendQuery(lua_State* L, IQ::Type type) {
	SluiftClient* client = getClient(L);

	JID to;
	if (boost::optional<std::string> toString = Lua::getStringField(L, 2, "to")) {
		to = JID(*toString);
	}

	int timeout = Sluift::globals.timeout;
	if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
		timeout = *timeoutInt;
	}

	boost::shared_ptr<Payload> payload;
	lua_getfield(L, 2, "query");
	if (lua_type(L, -1) == LUA_TTABLE) {
		payload = Sluift::globals.elementConvertor.convertFromLua(L, -1);
	}
	else if (lua_type(L, -1) == LUA_TSTRING) {
		payload = boost::make_shared<RawXMLPayload>(Lua::checkString(L, -1));
	}
	lua_pop(L, 1);

	return client->sendRequest(
		boost::make_shared< GenericRequest<Payload> >(type, to, payload, client->getClient()->getIQRouter()), timeout).convertToLuaResult(L);
}

#define DISPATCH_PUBSUB_PAYLOAD(payloadType, container, response) \
	else if (boost::shared_ptr<payloadType> p = boost::dynamic_pointer_cast<payloadType>(payload)) { \
		return client->sendPubSubRequest(type, to, p, timeout).convertToLuaResult(L); \
	}

SLUIFT_LUA_FUNCTION(Client, query_pubsub) {
	SluiftClient* client = getClient(L);

	JID to;
	if (boost::optional<std::string> toString = Lua::getStringField(L, 2, "to")) {
		to = JID(*toString);
	}

	int timeout = Sluift::globals.timeout;
	if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
		timeout = *timeoutInt;
	}

	IQ::Type type;
	if (boost::optional<std::string> queryType = Lua::getStringField(L, 2, "type")) {
		if (*queryType == "get") {
			type = IQ::Get;
		}
		else if (*queryType == "set") {
			type = IQ::Set;
		}
		else {
			throw Lua::Exception("Illegal query type: '" + *queryType + "'");
		}
	}
	else {
		throw Lua::Exception("Missing query type");
	}

	lua_getfield(L, 2, "query");
	if (!lua_istable(L, -1)) {
		throw Lua::Exception("Missing/incorrect query");
	}
	boost::shared_ptr<Payload> payload = Sluift::globals.elementConvertor.convertFromLua(L, -1);

	if (false) { }
	SWIFTEN_PUBSUB_FOREACH_PUBSUB_PAYLOAD_TYPE(DISPATCH_PUBSUB_PAYLOAD)
	else {
		throw Lua::Exception("Incorrect PubSub payload");
	}
}


SLUIFT_LUA_FUNCTION(Client, get) {
	return sendQuery(L, IQ::Get);
}

SLUIFT_LUA_FUNCTION(Client, set) {
	return sendQuery(L, IQ::Set);
}

SLUIFT_LUA_FUNCTION(Client, send) {
	Sluift::globals.eventLoop.runOnce();

	getClient(L)->getClient()->sendData(std::string(Lua::checkString(L, 2)));
	lua_pushvalue(L, 1);
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, set_options) {
	SluiftClient* client = getClient(L);
	Lua::checkType(L, 2, LUA_TTABLE);
	lua_getfield(L, 2, "ack");
	if (!lua_isnil(L, -1)) {
		client->getOptions().useAcks = lua_toboolean(L, -1);
	}
	lua_getfield(L, 2, "compress");
	if (!lua_isnil(L, -1)) {
		client->getOptions().useStreamCompression = lua_toboolean(L, -1);
	}
	lua_getfield(L, 2, "tls");
	if (!lua_isnil(L, -1)) {
		bool useTLS = lua_toboolean(L, -1);
		client->getOptions().useTLS = (useTLS ? ClientOptions::UseTLSWhenAvailable : ClientOptions::NeverUseTLS);
	}
	lua_pushvalue(L, 1);
	return 0;
}

static void pushEvent(lua_State* L, const SluiftClient::Event& event) {
	switch (event.type) {
		case SluiftClient::Event::MessageType: {
			Message::ref message = boost::dynamic_pointer_cast<Message>(event.stanza);
			Lua::Table result = boost::assign::map_list_of
				("type", boost::make_shared<Lua::Value>(std::string("message")))
				("from", boost::make_shared<Lua::Value>(message->getFrom().toString()))
				("body", boost::make_shared<Lua::Value>(message->getBody()));
			Lua::pushValue(L, result);
			Lua::registerTableToString(L, -1);
			break;
		}
		case SluiftClient::Event::PresenceType: {
			Presence::ref presence = boost::dynamic_pointer_cast<Presence>(event.stanza);
			Lua::Table result = boost::assign::map_list_of
				("type", boost::make_shared<Lua::Value>(std::string("presence")))
				("from", boost::make_shared<Lua::Value>(presence->getFrom().toString()))
				("status", boost::make_shared<Lua::Value>(presence->getStatus()));
			Lua::pushValue(L, result);
			if (!presence->getPayloads().empty()) {
				const std::vector<boost::shared_ptr<Payload> > payloads = presence->getPayloads();
				lua_createtable(L, boost::numeric_cast<int>(payloads.size()), 0);
				for (size_t i = 0; i < payloads.size(); ++i) {
					Sluift::globals.elementConvertor.convertToLua(L, payloads[i]);
					lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
				}
				Lua::registerGetByTypeIndex(L, -1);
				lua_setfield(L, -2, "payloads");
			}
			Lua::registerTableToString(L, -1);
			break;
		}
		case SluiftClient::Event::PubSubEventType: {
			Sluift::globals.elementConvertor.convertToLua(L, event.pubsubEvent);
			lua_pushstring(L, "pubsub");
			lua_setfield(L, -2, "type");
			lua_pushstring(L, event.from.toString().c_str());
			lua_setfield(L, -2, "from");

			lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.bootIndex);
			lua_getfield(L, -1, "process_pubsub_event");
			lua_pushvalue(L, -3);
			lua_call(L, 1, 0);
			lua_pop(L, 1);
		}
	}
}

struct CallUnaryLuaPredicateOnEvent {
	CallUnaryLuaPredicateOnEvent(lua_State* L, int index) : L(L), index(index) {
	}

	bool operator()(const SluiftClient::Event& event) {
		lua_pushvalue(L, index);
		pushEvent(L, event);
		if (lua_pcall(L, 1, 1, 0) != 0) {
			throw Lua::Exception(lua_tostring(L, -1));
		}
		bool result = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return result;
	}

	lua_State* L;
	int index;
};


SLUIFT_LUA_FUNCTION(Client, get_next_event) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);

	int timeout = Sluift::globals.timeout;
	boost::optional<SluiftClient::Event::Type> type;
	int condition = 0;
	if (lua_istable(L, 2)) {
		if (boost::optional<std::string> typeString = Lua::getStringField(L, 2, "type")) {
			if (*typeString == "message") {
				type = SluiftClient::Event::MessageType;
			}
			else if (*typeString == "presence") {
				type = SluiftClient::Event::PresenceType;
			}
			else if (*typeString == "pubsub") {
				type = SluiftClient::Event::PubSubEventType;
			}
		}
		if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
			timeout = *timeoutInt;
		}
		lua_getfield(L, 2, "if");
		if (lua_isfunction(L, -1)) {
			condition = Lua::absoluteOffset(L, -1);
		}
	}

	boost::optional<SluiftClient::Event> event;
	if (condition) {
		event = client->getNextEvent(timeout, CallUnaryLuaPredicateOnEvent(L, condition));
	}
	else if (type) {
		event = client->getNextEvent(
				timeout, lambda::bind(&SluiftClient::Event::type, lambda::_1) == *type);
	}
	else {
		event = client->getNextEvent(timeout);
	}

	if (event) {
		pushEvent(L, *event);
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}


SLUIFT_LUA_FUNCTION(Client, add_contact) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	RosterItemPayload item;
	if (lua_type(L, 2) == LUA_TTABLE) {
		lua_getfield(L, 2, "jid");
		const char* rawJID = lua_tostring(L, -1);
		if (rawJID) {
			item.setJID(std::string(rawJID));
		}
		lua_getfield(L, 2, "name");
		const char* rawName = lua_tostring(L, -1);
		if (rawName) {
			item.setName(rawName);
		}
		lua_getfield(L, 2, "groups");
		if (!lua_isnil(L, -1)) {
			if (lua_type(L, -1) == LUA_TTABLE) {
				for (size_t i = 1; i <= lua_objlen(L, -1); ++i) {
					lua_rawgeti(L, -1, boost::numeric_cast<int>(i));
					const char* rawGroup = lua_tostring(L, -1);
					if (rawGroup) {
						item.addGroup(rawGroup);
					}
					lua_pop(L, 1);
				}
			}
			else {
				throw Lua::Exception("Groups should be a table");
			}
		}
	}
	else {
		item.setJID(Lua::checkString(L, 2));
	}

	client->getRoster();
	if (!client->getClient()->getRoster()->containsJID(item.getJID())) {
		RosterPayload::ref roster = boost::make_shared<RosterPayload>();
		roster->addItem(item);

		Sluift::Response response = client->sendVoidRequest(
			SetRosterRequest::create(roster, client->getClient()->getIQRouter()), -1);
		if (response.error) {
			return response.convertToLuaResult(L);
		}
	}
	client->getClient()->getSubscriptionManager()->requestSubscription(item.getJID());
	lua_pushboolean(L, true);
	return 1;
}

SLUIFT_LUA_FUNCTION(Client, remove_contact) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	JID jid(Lua::checkString(L, 2));

	RosterPayload::ref roster = boost::make_shared<RosterPayload>();
	roster->addItem(RosterItemPayload(JID(Lua::checkString(L, 2)), "", RosterItemPayload::Remove));
		
	return client->sendVoidRequest(
		SetRosterRequest::create(roster, client->getClient()->getIQRouter()), -1).convertToLuaResult(L);
}

SLUIFT_LUA_FUNCTION(Client, confirm_subscription) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	JID jid(Lua::checkString(L, 2));
	client->getClient()->getSubscriptionManager()->confirmSubscription(jid);
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, cancel_subscription) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	JID jid(Lua::checkString(L, 2));
	client->getClient()->getSubscriptionManager()->cancelSubscription(jid);
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, set_disco_info) {
	SluiftClient* client = getClient(L);
	if (!lua_istable(L, 2)) {
		throw Lua::Exception("Missing disco info");
	}
	if (boost::shared_ptr<DiscoInfo> discoInfo = boost::dynamic_pointer_cast<DiscoInfo>(Sluift::globals.elementConvertor.convertFromLuaUntyped(L, 2, "disco_info"))) {
		client->getClient()->getDiscoManager()->setDiscoInfo(*discoInfo);
	}
	else {
		throw Lua::Exception("Illegal disco info");
	}
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, set_caps_node) {
	SluiftClient* client = getClient(L);
	std::string node(Lua::checkString(L, 2));
	client->getClient()->getDiscoManager()->setCapsNode(Lua::checkString(L, 2));
	return 0;
}

SLUIFT_LUA_FUNCTION(Client, jid) {
	SluiftClient* client = getClient(L);
	lua_pushstring(L, client->getClient()->getJID().toString().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(Client, __gc) {
	SluiftClient* client = getClient(L);
	delete client;
	return 0;
}
