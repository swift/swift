/*
 * Copyright (c) 2013-2014 Isode Limited.
 * All rights reserved.
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
#include <Swiften/Elements/MAMQuery.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Roster/SetRosterRequest.h>
#include <Swiften/Presence/SubscriptionManager.h>
#include <Swiften/Roster/XMPPRosterItem.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>
#include <Sluift/Lua/FunctionRegistration.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/IDGenerator.h>
#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/Value.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/globals.h>
#include <Sluift/ElementConvertors/StanzaConvertor.h>
#include <Sluift/ElementConvertors/IQConvertor.h>
#include <Sluift/ElementConvertors/PresenceConvertor.h>
#include <Sluift/ElementConvertors/MessageConvertor.h>
#include <Sluift/ElementConvertors/StatusShowConvertor.h>

using namespace Swift;
namespace lambda = boost::lambda;

static inline SluiftClient* getClient(lua_State* L) {
	return *Lua::checkUserData<SluiftClient>(L, 1);
}

static inline int getGlobalTimeout(lua_State* L) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.moduleLibIndex);
	lua_getfield(L, -1, "timeout");
	int result = boost::numeric_cast<int>(lua_tointeger(L, -1));
	lua_pop(L, 2);
	return result;
}

static void addPayloadsToTable(lua_State* L, const std::vector<boost::shared_ptr<Payload> >& payloads) {
	if (!payloads.empty()) {
		lua_createtable(L, boost::numeric_cast<int>(payloads.size()), 0);
		for (size_t i = 0; i < payloads.size(); ++i) {
			Sluift::globals.elementConvertor.convertToLua(L, payloads[i]);
			lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
		}
		Lua::registerGetByTypeIndex(L, -1);
		lua_setfield(L, -2, "payloads");
	}
}

static boost::shared_ptr<Payload> getPayload(lua_State* L, int index) {
	if (lua_type(L, index) == LUA_TTABLE) {
		return boost::dynamic_pointer_cast<Payload>(Sluift::globals.elementConvertor.convertFromLua(L, index));
	}
	else if (lua_type(L, index) == LUA_TSTRING) {
		return boost::make_shared<RawXMLPayload>(Lua::checkString(L, index));
	}
	else {
		return boost::shared_ptr<Payload>();
	}
}

static std::vector< boost::shared_ptr<Payload> > getPayloadsFromTable(lua_State* L, int index) {
	index = Lua::absoluteOffset(L, index);
	std::vector< boost::shared_ptr<Payload> > result;
	lua_getfield(L, index, "payloads");
	if (lua_istable(L, -1)) {
		for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
			boost::shared_ptr<Payload> payload = getPayload(L, -1);
			if (payload) {
				result.push_back(payload);
			}
		}
	}
	lua_pop(L, 1);
	return result;
}

SLUIFT_LUA_FUNCTION(Client, async_connect) {
	SluiftClient* client = getClient(L);

	std::string host = client->getOptions().manualHostname;
	int port = client->getOptions().manualPort;
	if (lua_istable(L, 2)) {
		if (boost::optional<std::string> hostString = Lua::getStringField(L, 2, "host")) {
			host = *hostString;
		}
		if (boost::optional<int> portInt = Lua::getIntField(L, 2, "port")) {
			port = *portInt;
		}
	}
	client->connect(host, port);
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, set_trace_enabled,
		"Enable/disable tracing of the data sent/received.\n\n.",
		"self\n"
		"enable  a boolean specifying whether to enable/disable tracing",
		""
) {
	getClient(L)->setTraceEnabled(lua_toboolean(L, 1));
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, wait_connected,
		"Block until the client is connected.\n\nThis is useful after an `async_connect`.",
		"self",
		""
) {
	getClient(L)->waitConnected(getGlobalTimeout(L));
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, is_connected,
		"Checks whether this client is still connected.\n\nReturns a boolean.",
		"self\n",
		""
) {
	lua_pushboolean(L, getClient(L)->isConnected());
	return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, disconnect,
		"Disconnect from the server",
		"self\n",
		""
) {
	Sluift::globals.eventLoop.runOnce();
	getClient(L)->disconnect();
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, set_version,

		"Sets the published version of this client.",

		"self",

		"name  the name of the client software\n"
		"version  the version identifier of this client\n"
		"os  the OS this client is running on\n"
) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	if (boost::shared_ptr<SoftwareVersion> version = boost::dynamic_pointer_cast<SoftwareVersion>(Sluift::globals.elementConvertor.convertFromLuaUntyped(L, 2, "software_version"))) {
		client->setSoftwareVersion(version->getName(), version->getVersion(), version->getOS());
	}
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, get_contacts,
		"Returns a table of all the contacts in the contact list.",
		"self\n",
		""
) {
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

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, send_message,
		"Send a message.",
		"self\n"
		"to  the JID to send the message to\n"
		"body  the body of the message. Can alternatively be specified using the `body` option\n",

		"to  the JID to send the message to\n"
		"body  the body of the message\n"
		"subject  the subject of the MUC room to set\n"
		"type  the type of message to send (`normal`, `chat`, `error`, `groupchat`, `headline`)\n"
		"payloads  payloads to add to the message\n"
) {
	Sluift::globals.eventLoop.runOnce();
	JID to;
	boost::optional<std::string> body;
	boost::optional<std::string> subject;
	std::vector<boost::shared_ptr<Payload> > payloads;
	int index = 2;
	Message::Type type = Message::Chat;
	if (lua_isstring(L, index)) {
		to = std::string(lua_tostring(L, index));
		++index;
		if (lua_isstring(L, index)) {
			body = lua_tostring(L, index);
			++index;
		}
	}
	if (lua_istable(L, index)) {
		if (boost::optional<std::string> value = Lua::getStringField(L, index, "to")) {
			to = *value;
		}

		if (boost::optional<std::string> value = Lua::getStringField(L, index, "body")) {
			body = value;
		}

		if (boost::optional<std::string> value = Lua::getStringField(L, index, "type")) {
			type = MessageConvertor::convertMessageTypeFromString(*value);
		}

		if (boost::optional<std::string> value = Lua::getStringField(L, index, "subject")) {
			subject = value;
		}

		payloads = getPayloadsFromTable(L, index);
	}

	if (!to.isValid()) {
		throw Lua::Exception("Missing 'to'");
	}
	if ((!body || body->empty()) && !subject && payloads.empty()) {
		throw Lua::Exception("Missing any of 'body', 'subject' or 'payloads'");
	}
	Message::ref message = boost::make_shared<Message>();
	message->setTo(to);
	if (body && !body->empty()) {
		message->setBody(*body);
	}
	if (subject) {
		message->setSubject(*subject);
	}
	message->addPayloads(payloads.begin(), payloads.end());
	message->setType(type);
	getClient(L)->getClient()->sendMessage(message);
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, send_presence,
		"Send presence.",

		"self\n"
		"body  the text of the presence. Can alternatively be specified using the `status` option\n",

		"to  the JID to send the message to\n"
		"status  the text of the presence\n"
		"show  the availability of the presence (`online`, `ffc`, `away`, `xa`, `dnd`)\n"
		"priority  the priority of the presence\n"
		"type  the type of message to send (`available`, `error`, `probe`, `subscribe`, `subscribed`, `unavailable`, `unsubscribe`, `unsubscribed`)\n"
		"payloads  payloads to add to the presence\n"
) {
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
			presence->setType(PresenceConvertor::convertPresenceTypeFromString(*value));
		}
		if (boost::optional<std::string> value = Lua::getStringField(L, index, "show")) {
			presence->setShow(StatusShowConvertor::convertStatusShowTypeFromString(*value));
		}
		std::vector< boost::shared_ptr<Payload> > payloads = getPayloadsFromTable(L, index);
		presence->addPayloads(payloads.begin(), payloads.end());
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

	int timeout = getGlobalTimeout(L);
	if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
		timeout = *timeoutInt;
	}

	boost::shared_ptr<Payload> payload;
	lua_getfield(L, 2, "query");
	payload = getPayload(L, -1);
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

	int timeout = getGlobalTimeout(L);
	if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
		timeout = *timeoutInt;
	}

	IQ::Type type;
	if (boost::optional<std::string> queryType = Lua::getStringField(L, 2, "type")) {
		type = IQConvertor::convertIQTypeFromString(*queryType);
	}
	else {
		throw Lua::Exception("Missing query type");
	}

	lua_getfield(L, 2, "query");
	if (!lua_istable(L, -1)) {
		throw Lua::Exception("Missing/incorrect query");
	}
	boost::shared_ptr<Payload> payload = getPayload(L, -1);

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

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, send,
		"Sends a raw string",

		"self\n"
		"data  the string to send\n",

		""
) {
	Sluift::globals.eventLoop.runOnce();

	getClient(L)->getClient()->sendData(std::string(Lua::checkString(L, 2)));
	lua_pushvalue(L, 1);
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, set_options,

		"Sets the connection options of this client.",

		"self",

		"host  The host to connect to. When omitted, is determined from resolving the JID domain.\n"
		"port  The port to connect to. When omitted, is determined from resolving the JID domain.\n"
		"ack  Request acknowledgements\n"
		"compress  Use stream compression when available\n"
		"tls  Use TLS when available\n"
		"bosh_url  Connect using the specified BOSH URL\n"
		"allow_plain_without_tls  Allow PLAIN authentication without a TLS encrypted connection\n"
) {
	SluiftClient* client = getClient(L);
	Lua::checkType(L, 2, LUA_TTABLE);
	lua_getfield(L, 2, "host");
	if (!lua_isnil(L, -1)) {
		client->getOptions().manualHostname = lua_tostring(L, -1);
	}
	lua_getfield(L, 2, "port");
	if (!lua_isnil(L, -1)) {
		client->getOptions().manualPort = boost::numeric_cast<int>(lua_tointeger(L, -1));
	}
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
	lua_getfield(L, 2, "bosh_url");
	if (!lua_isnil(L, -1)) {
		client->getOptions().boshURL = URL::fromString(lua_tostring(L, -1));
	}
	lua_getfield(L, 2, "allow_plain_without_tls");
	if (!lua_isnil(L, -1)) {
		client->getOptions().allowPLAINWithoutTLS = lua_toboolean(L, -1);
	}
	lua_pushvalue(L, 1);
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, get_options,
		"Returns a table with all the connection options of this client.",
		"self\n",
		""
) {
	Sluift::globals.eventLoop.runOnce();

	SluiftClient* client = getClient(L);
	Lua::Table optionsTable = boost::assign::map_list_of
		("host", boost::make_shared<Lua::Value>(client->getOptions().manualHostname))
		("port", boost::make_shared<Lua::Value>(client->getOptions().manualPort))
		("ack", boost::make_shared<Lua::Value>(client->getOptions().useAcks))
		("compress", boost::make_shared<Lua::Value>(client->getOptions().useStreamCompression))
		("tls", boost::make_shared<Lua::Value>(client->getOptions().useTLS == ClientOptions::NeverUseTLS ? false : true))
		("bosh_url", boost::make_shared<Lua::Value>(client->getOptions().boshURL.toString()))
		("allow_plain_without_tls", boost::make_shared<Lua::Value>(client->getOptions().allowPLAINWithoutTLS));
	pushValue(L, optionsTable);
	Lua::registerTableToString(L, -1);
	return 1;
}

static void pushEvent(lua_State* L, const SluiftClient::Event& event) {
	switch (event.type) {
		case SluiftClient::Event::MessageType: {
			Message::ref message = boost::dynamic_pointer_cast<Message>(event.stanza);
			Lua::Table result = boost::assign::map_list_of
				("type", boost::make_shared<Lua::Value>(std::string("message")))
				("from", boost::make_shared<Lua::Value>(message->getFrom().toString()))
				("body", boost::make_shared<Lua::Value>(message->getBody()))
				("message_type", boost::make_shared<Lua::Value>(MessageConvertor::convertMessageTypeToString(message->getType())));
			Lua::pushValue(L, result);
			addPayloadsToTable(L, message->getPayloads());
			Lua::registerTableToString(L, -1);
			break;
		}
		case SluiftClient::Event::PresenceType: {
			Presence::ref presence = boost::dynamic_pointer_cast<Presence>(event.stanza);
			Lua::Table result = boost::assign::map_list_of
				("type", boost::make_shared<Lua::Value>(std::string("presence")))
				("from", boost::make_shared<Lua::Value>(presence->getFrom().toString()))
				("status", boost::make_shared<Lua::Value>(presence->getStatus()))
				("presence_type", boost::make_shared<Lua::Value>(PresenceConvertor::convertPresenceTypeToString(presence->getType())));
			Lua::pushValue(L, result);
			addPayloadsToTable(L, presence->getPayloads());
			Lua::registerTableToString(L, -1);
			break;
		}
		case SluiftClient::Event::PubSubEventType: {
			Sluift::globals.elementConvertor.convertToLua(L, event.pubsubEvent);
			lua_pushstring(L, "pubsub");
			lua_setfield(L, -2, "type");
			lua_pushstring(L, event.from.toString().c_str());
			lua_setfield(L, -2, "from");

			lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
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

	int timeout = getGlobalTimeout(L);
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


SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, add_contact,
		"Add a contact to the contact list.",
		"self\n",
		"jid  The JID of the contact to add\n"
		"name  The name to use in the contact list\n"
		"groups  An array of group names to add the contact to\n") {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	RosterItemPayload item;
	int timeout = getGlobalTimeout(L);

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
			SetRosterRequest::create(roster, client->getClient()->getIQRouter()), timeout);
		if (response.error) {
			return response.convertToLuaResult(L);
		}
	}
	client->getClient()->getSubscriptionManager()->requestSubscription(item.getJID());
	lua_pushboolean(L, true);
	return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, remove_contact,
		"Remove a contact from the contact list.",
		"self\n"
		"jid  the JID of the contact to remove\n",
		""
) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	JID jid(Lua::checkString(L, 2));
	int timeout = getGlobalTimeout(L);

	RosterPayload::ref roster = boost::make_shared<RosterPayload>();
	roster->addItem(RosterItemPayload(JID(Lua::checkString(L, 2)), "", RosterItemPayload::Remove));
		
	return client->sendVoidRequest(
		SetRosterRequest::create(roster, client->getClient()->getIQRouter()), timeout).convertToLuaResult(L);
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, confirm_subscription,
		"Confirm subscription of a contact.",
		"self\n"
		"jid  the JID of the contact to confirm the subscription of\n",
		""
) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	JID jid(Lua::checkString(L, 2));
	client->getClient()->getSubscriptionManager()->confirmSubscription(jid);
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, cancel_subscription,
		"Cancel the subscription of a contact.",
		"self\n"
		"jid  the JID of the contact to cancel the subscription of\n",
		""
) {
	Sluift::globals.eventLoop.runOnce();
	SluiftClient* client = getClient(L);
	JID jid(Lua::checkString(L, 2));
	client->getClient()->getSubscriptionManager()->cancelSubscription(jid);
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, set_disco_info,
		"Sets the service discovery information for this client",
		"self\n"
		"disco_info  A structured representation of the service discovery information\n",
		""
) {
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

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, set_caps_node,
		"Sets the caps node of this client",
		"self\n"
		"node  The caps node (e.g. 'http://swift.im/sluift')\n",
		""
) {
	SluiftClient* client = getClient(L);
	std::string node(Lua::checkString(L, 2));
	client->getClient()->getDiscoManager()->setCapsNode(Lua::checkString(L, 2));
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Client, jid,
		"Returns the JID of this client",
		"self\n",
		""
) {
	SluiftClient* client = getClient(L);
	lua_pushstring(L, client->getClient()->getJID().toString().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(Client, __gc) {
	SluiftClient* client = getClient(L);
	delete client;
	return 0;
}
