/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "sluift.h"
#include <lauxlib.h>

#include <iostream>
#include <string>
#include <deque>
#include <boost/assign/list_of.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Swiften.h>

#include "Watchdog.h"
#include "SluiftException.h"
#include "ResponseSink.h"
#include "Lua/Value.h"

using namespace Swift;

#define SLUIFT_CLIENT "SluiftClient*"

/*******************************************************************************
 * Forward declarations
 ******************************************************************************/

static bool debug = false;
static int globalTimeout = 30000;

/*******************************************************************************
 * Helper classes
 ******************************************************************************/

SimpleEventLoop eventLoop;
BoostNetworkFactories networkFactories(&eventLoop);


class SluiftClient {
	public:
		SluiftClient(const JID& jid, const std::string& password) : tracer(NULL) {
			client = new Client(jid, password, &networkFactories);
			client->setAlwaysTrustCertificates();
			client->onDisconnected.connect(boost::bind(&SluiftClient::handleDisconnected, this, _1));
			client->onMessageReceived.connect(boost::bind(&SluiftClient::handleIncomingEvent, this, _1));
			client->onPresenceReceived.connect(boost::bind(&SluiftClient::handleIncomingEvent, this, _1));
			client->getRoster()->onInitialRosterPopulated.connect(boost::bind(&SluiftClient::handleInitialRosterPopulated, this));
			if (debug) {
				tracer = new ClientXMLTracer(client);
			}
		}

		~SluiftClient() {
			delete tracer;
			delete client;
		}

		Client* getClient() {
			return client;
		}

		ClientOptions& getOptions() {
			return options;
		}

		void connect() {
			rosterReceived = false;
			client->connect();
		}

		void connect(const std::string& host) {
			rosterReceived = false;
			client->connect(host);
		}

		void waitConnected() {
			Watchdog watchdog(globalTimeout, networkFactories.getTimerFactory());
			while (!watchdog.getTimedOut() && client->isActive() && !client->isAvailable()) {
				eventLoop.runUntilEvents();
			}
			if (watchdog.getTimedOut()) {
				client->disconnect();
				throw SluiftException("Timeout while connecting");
			}
		}

		bool isConnected() const {
			return client->isAvailable();
		}

		void sendMessage(const JID& to, const std::string& body) {
			Message::ref message = boost::make_shared<Message>();
			message->setTo(to);
			message->setBody(body);
			client->sendMessage(message);
		}

		void sendPresence(const std::string& status) {
			client->sendPresence(boost::make_shared<Presence>(status));
		}

		boost::optional<std::string> sendQuery(const JID& jid, IQ::Type type, const std::string& data, int timeout) {
			rawRequestResponse.reset();
			RawRequest::ref request = RawRequest::create(type, jid, data, client->getIQRouter());
			boost::signals::scoped_connection c = request->onResponse.connect(boost::bind(&SluiftClient::handleRawRequestResponse, this, _1));
			request->send();

			Watchdog watchdog(timeout, networkFactories.getTimerFactory());
			while (!watchdog.getTimedOut() && !rawRequestResponse) {
				eventLoop.runUntilEvents();
			}

			if (watchdog.getTimedOut()) {
				return boost::optional<std::string>();
			}
			else {
				return *rawRequestResponse;
			}
		}

		void disconnect() {
			client->disconnect();
			while (client->isActive()) {
				eventLoop.runUntilEvents();
			}
		}

		void setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os) {
			client->setSoftwareVersion(name, version, os);
		}

		Stanza::ref getNextEvent(int timeout) {
			if (!pendingEvents.empty()) {
				Stanza::ref event = pendingEvents.front();
				pendingEvents.pop_front();
				return event;
			}
			Watchdog watchdog(timeout, networkFactories.getTimerFactory());
			while (!watchdog.getTimedOut() && pendingEvents.empty() && !client->isActive()) {
				eventLoop.runUntilEvents();
			}
			if (watchdog.getTimedOut() || !client->isActive()) {
				return Stanza::ref();
			}
			else if (!pendingEvents.empty()) {
				Stanza::ref event = pendingEvents.front();
				pendingEvents.pop_front();
				return event;
			}
			else {
				return Stanza::ref();
			}
		}

		std::vector<XMPPRosterItem> getRoster() {
			if (!rosterReceived) {
				// If we haven't requested it yet, request it for the first time
				client->requestRoster();
			}
			while (!rosterReceived) {
				eventLoop.runUntilEvents();
			}
			return client->getRoster()->getItems();
		}

	private:
		void handleIncomingEvent(Stanza::ref stanza) {
			pendingEvents.push_back(stanza);
		}

		void handleInitialRosterPopulated() {
			rosterReceived = true;
		}

		void handleRawRequestResponse(const std::string& response) {
			rawRequestResponse = response;
		}

		void handleDisconnected(const boost::optional<ClientError>& error) {
			if (error) {
				throw SluiftException(*error);
			}
		}
	
	private:
		Client* client;
		ClientOptions options;
		ClientXMLTracer* tracer;
		bool rosterReceived;
		std::deque<Stanza::ref> pendingEvents;
		boost::optional<std::string> rawRequestResponse;
};

/*******************************************************************************
 * Client functions.
 ******************************************************************************/

static inline SluiftClient* getClient(lua_State* L) {
	return *reinterpret_cast<SluiftClient**>(luaL_checkudata(L, 1, SLUIFT_CLIENT));
}

static int sluift_client_connect(lua_State *L) {
	try {
		SluiftClient* client = getClient(L);
		std::string host(luaL_checkstring(L, 2));
		if (host.empty()) {
			client->connect();
		}
		else {
			client->connect(host);
		}
		client->waitConnected();
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_async_connect(lua_State *L) {
	try {
		getClient(L)->connect();
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_wait_connected(lua_State *L) {
	try {
		getClient(L)->waitConnected();
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_is_connected(lua_State *L) {
	lua_pushboolean(L, getClient(L)->isConnected());
	return 1;
}

static int sluift_client_disconnect(lua_State *L) {
	try {
		getClient(L)->disconnect();
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_set_version(lua_State *L) {
	try {
		eventLoop.runOnce();

		SluiftClient* client = getClient(L);
		luaL_checktype(L, 2, LUA_TTABLE);
		lua_getfield(L, 2, "name");
		const char* rawName = lua_tostring(L, -1);
		lua_getfield(L, 2, "version");
		const char* rawVersion = lua_tostring(L, -1);
		lua_getfield(L, 2, "os");
		const char* rawOS = lua_tostring(L, -1);
		client->setSoftwareVersion(rawName ? rawName : "", rawVersion ? rawVersion : "", rawOS ? rawOS : "");
		lua_pop(L, 3);
		lua_pushvalue(L, 1);
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_get_contacts(lua_State *L) {
	try {
		eventLoop.runOnce();

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
			Lua::Value groups(std::vector<Lua::Value>(item.getGroups().begin(), item.getGroups().end()));
			Lua::Table itemTable = boost::assign::map_list_of
				("jid", boost::make_shared<Lua::Value>(item.getJID().toString()))
				("name", boost::make_shared<Lua::Value>(item.getName()))
				("subscription", boost::make_shared<Lua::Value>(subscription))
				("groups", boost::make_shared<Lua::Value>(std::vector<Lua::Value>(item.getGroups().begin(), item.getGroups().end())));
			contactsTable[item.getJID().toString()] = boost::make_shared<Lua::Value>(itemTable);
		}
		pushValue(L, contactsTable);
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_get_version(lua_State *L) {
	try {
		SluiftClient* client = getClient(L);
		int timeout = -1;
		if (lua_type(L, 3) != LUA_TNONE) {
			timeout = luaL_checknumber(L, 3);
		}

		ResponseSink<SoftwareVersion> sink;
		GetSoftwareVersionRequest::ref request = GetSoftwareVersionRequest::create(std::string(luaL_checkstring(L, 2)), client->getClient()->getIQRouter());
		boost::signals::scoped_connection c = request->onResponse.connect(boost::ref(sink));
		request->send();

		Watchdog watchdog(timeout, networkFactories.getTimerFactory());
		while (!watchdog.getTimedOut() && !sink.hasResponse()) {
			eventLoop.runUntilEvents();
		}

		ErrorPayload::ref error = sink.getResponseError();
		if (error || watchdog.getTimedOut()) {
			lua_pushnil(L);
			if (watchdog.getTimedOut()) {
				lua_pushstring(L, "Timeout");
			}
			else if (error->getCondition() == ErrorPayload::RemoteServerNotFound) {
				lua_pushstring(L, "Remote server not found");
			}
			// TODO
			else {
				lua_pushstring(L, "Error");
			}
			return 2;
		}
		else if (SoftwareVersion::ref version = sink.getResponsePayload()) {
			Lua::Table result = boost::assign::map_list_of
				("name", boost::make_shared<Lua::Value>(version->getName()))
				("version", boost::make_shared<Lua::Value>(version->getVersion()))
				("os", boost::make_shared<Lua::Value>(version->getOS()));
			Lua::pushValue(L, result);
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_send_message(lua_State *L) {
	try {
		eventLoop.runOnce();

		getClient(L)->sendMessage(std::string(luaL_checkstring(L, 2)), luaL_checkstring(L, 3));
		lua_pushvalue(L, 1);
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_send_presence(lua_State *L) {
	try {
		eventLoop.runOnce();

		getClient(L)->sendPresence(std::string(luaL_checkstring(L, 2)));
		lua_pushvalue(L, 1);
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_get(lua_State *L) {
	try {
		SluiftClient* client = getClient(L);
		JID jid;
		std::string data;
		int timeout = -1;
		if (lua_type(L, 3) == LUA_TSTRING) {
			jid = JID(std::string(luaL_checkstring(L, 2)));
			data = std::string(luaL_checkstring(L, 3));
			if (lua_type(L, 4) != LUA_TNONE) {
				timeout = luaL_checknumber(L, 4);
			}
		}
		else {
			data = std::string(luaL_checkstring(L, 2));
			if (lua_type(L, 3) != LUA_TNONE) {
				timeout = luaL_checknumber(L, 3);
			}
		}
		boost::optional<std::string> result = client->sendQuery(jid, IQ::Get, data, timeout);
		if (result) {
			lua_pushstring(L, result->c_str());
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_set(lua_State *L) {
	try {
		SluiftClient* client = getClient(L);
		JID jid;
		std::string data;
		int timeout = -1;
		if (lua_type(L, 3) == LUA_TSTRING) {
			jid = JID(std::string(luaL_checkstring(L, 2)));
			data = std::string(luaL_checkstring(L, 3));
			if (lua_type(L, 4) != LUA_TNONE) {
				timeout = luaL_checknumber(L, 4);
			}
		}
		else {
			data = std::string(luaL_checkstring(L, 2));
			if (lua_type(L, 3) != LUA_TNONE) {
				timeout = luaL_checknumber(L, 3);
			}
		}
		boost::optional<std::string> result = client->sendQuery(jid, IQ::Set, data, timeout);
		if (result) {
			lua_pushstring(L, result->c_str());
		}
		else {
			lua_pushnil(L);
		}
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_send(lua_State *L) {
	try {
		eventLoop.runOnce();

		getClient(L)->getClient()->sendData(std::string(luaL_checkstring(L, 2)));
		lua_pushvalue(L, 1);
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_set_options(lua_State* L) {
	SluiftClient* client = getClient(L);
	luaL_checktype(L, 2, LUA_TTABLE);
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

static void pushEvent(lua_State* L, Stanza::ref event) {
	if (Message::ref message = boost::dynamic_pointer_cast<Message>(event)) {
		Lua::Table result = boost::assign::map_list_of
			("type", boost::make_shared<Lua::Value>(std::string("message")))
			("from", boost::make_shared<Lua::Value>(message->getFrom().toString()))
			("body", boost::make_shared<Lua::Value>(message->getBody()));
		Lua::pushValue(L, result);
	}
	else if (Presence::ref presence = boost::dynamic_pointer_cast<Presence>(event)) {
		Lua::Table result = boost::assign::map_list_of
			("type", boost::make_shared<Lua::Value>(std::string("presence")))
			("from", boost::make_shared<Lua::Value>(presence->getFrom().toString()))
			("status", boost::make_shared<Lua::Value>(presence->getStatus()));
		Lua::pushValue(L, result);
	}
	else {
		lua_pushnil(L);
	}
}

static int sluift_client_for_event(lua_State *L) {
	try {
		eventLoop.runOnce();

		SluiftClient* client = getClient(L);
		luaL_checktype(L, 2, LUA_TFUNCTION);
		int timeout = -1;
		if (lua_type(L, 3) != LUA_TNONE) {
			timeout = lua_tonumber(L, 3);
		}

		while (true) {
			Stanza::ref event = client->getNextEvent(timeout);
			if (!event) {
				// We got a timeout
				lua_pushnil(L);
				return 1;
			}
			else {
				// Push the function and event on the stack
				lua_pushvalue(L, 2);
				pushEvent(L, event);
				int oldTop = lua_gettop(L) - 2;
				lua_call(L, 1, LUA_MULTRET);
				int returnValues = lua_gettop(L) - oldTop;
				if (returnValues > 0) {
					lua_remove(L, -1 - returnValues);
					return returnValues;
				}
			}
		}
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_get_next_event(lua_State *L) {
	try {
		eventLoop.runOnce();

		SluiftClient* client = getClient(L);
		int timeout = -1;
		if (lua_type(L, 2) != LUA_TNONE) {
			timeout = lua_tonumber(L, 2);
		}
		pushEvent(L, client->getNextEvent(timeout));
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}


static int sluift_client_add_contact(lua_State* L) {
	try {
		eventLoop.runOnce();
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
						lua_rawgeti(L, -1, i);
						const char* rawGroup = lua_tostring(L, -1);
						if (rawGroup) {
							item.addGroup(rawGroup);
						}
						lua_pop(L, 1);
					}
				}
				else {
					return luaL_error(L, "Groups should be a table");
				}
			}
		}
		else {
			item.setJID(luaL_checkstring(L, 2));
		}

		client->getRoster();
		if (!client->getClient()->getRoster()->containsJID(item.getJID())) {
			RosterPayload::ref roster = boost::make_shared<RosterPayload>();
			roster->addItem(item);

			ResponseSink<RosterPayload> sink;
			SetRosterRequest::ref request = SetRosterRequest::create(roster, client->getClient()->getIQRouter());
			boost::signals::scoped_connection c = request->onResponse.connect(boost::ref(sink));
			request->send();
			while (!sink.hasResponse()) {
				eventLoop.runUntilEvents();
			}
			if (sink.getResponseError()) {
				lua_pushboolean(L, false);
				return 1;
			}
		}
		client->getClient()->getSubscriptionManager()->requestSubscription(item.getJID());
		lua_pushboolean(L, true);
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_remove_contact(lua_State* L) {
	try {
		eventLoop.runOnce();
		SluiftClient* client = getClient(L);
		JID jid(luaL_checkstring(L, 2));

		RosterPayload::ref roster = boost::make_shared<RosterPayload>();
		roster->addItem(RosterItemPayload(JID(luaL_checkstring(L, 2)), "", RosterItemPayload::Remove));
		ResponseSink<RosterPayload> sink;
		SetRosterRequest::ref request = SetRosterRequest::create(roster, client->getClient()->getIQRouter());
		boost::signals::scoped_connection c = request->onResponse.connect(boost::ref(sink));
		request->send();
		while (!sink.hasResponse()) {
			eventLoop.runUntilEvents();
		}
		lua_pushboolean(L, !sink.getResponseError());
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_confirm_subscription(lua_State* L) {
	try {
		eventLoop.runOnce();
		SluiftClient* client = getClient(L);
		JID jid(luaL_checkstring(L, 2));
		client->getClient()->getSubscriptionManager()->confirmSubscription(jid);
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_cancel_subscription(lua_State* L) {
	try {
		eventLoop.runOnce();
		SluiftClient* client = getClient(L);
		JID jid(luaL_checkstring(L, 2));
		client->getClient()->getSubscriptionManager()->cancelSubscription(jid);
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_gc (lua_State *L) {
	SluiftClient* client = getClient(L);
	delete client;
	return 0;
}

static const luaL_reg sluift_client_functions[] = {
	{"connect",  sluift_client_connect},
	{"async_connect",  sluift_client_async_connect},
	{"wait_connected", sluift_client_wait_connected},
	{"is_connected", sluift_client_is_connected},
	{"disconnect",  sluift_client_disconnect},
	{"send_message", sluift_client_send_message},
	{"send_presence", sluift_client_send_presence},
	{"get", sluift_client_get},
	{"set", sluift_client_set},
	{"send", sluift_client_send},
	{"set_version", sluift_client_set_version},
	{"get_contacts", sluift_client_get_contacts},
	{"get_version", sluift_client_get_version},
	{"set_options", sluift_client_set_options},
	{"for_event", sluift_client_for_event},
	{"get_next_event", sluift_client_get_next_event},
	{"add_contact", sluift_client_add_contact},
	{"remove_contact", sluift_client_remove_contact},
	{"confirm_subscription", sluift_client_confirm_subscription},
	{"cancel_subscription", sluift_client_cancel_subscription},
	{"__gc", sluift_client_gc},
	{NULL, NULL}
};

/*******************************************************************************
 * Module functions
 ******************************************************************************/

static int sluift_new_client(lua_State *L) {
	try {
		JID jid(std::string(luaL_checkstring(L, 1)));
		std::string password(luaL_checkstring(L, 2));

		SluiftClient** client = reinterpret_cast<SluiftClient**>(lua_newuserdata(L, sizeof(SluiftClient*)));
		luaL_getmetatable(L, SLUIFT_CLIENT);
		lua_setmetatable(L, -2);

		*client = new SluiftClient(jid, password);
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_jid_to_bare(lua_State *L) {
	JID jid(std::string(luaL_checkstring(L, 1)));
	lua_pushstring(L, jid.toBare().toString().c_str());
	return 1;
}

static int sluift_jid_node(lua_State *L) {
	JID jid(std::string(luaL_checkstring(L, 1)));
	lua_pushstring(L, jid.getNode().c_str());
	return 1;
}

static int sluift_jid_domain(lua_State *L) {
	JID jid(std::string(luaL_checkstring(L, 1)));
	lua_pushstring(L, jid.getDomain().c_str());
	return 1;
}

static int sluift_jid_resource(lua_State *L) {
	JID jid(std::string(luaL_checkstring(L, 1)));
	lua_pushstring(L, jid.getResource().c_str());
	return 1;
}

static int sluift_sleep(lua_State *L) {
	try {
		eventLoop.runOnce();

		int timeout = luaL_checknumber(L, 1);
		Watchdog watchdog(timeout, networkFactories.getTimerFactory());
		while (!watchdog.getTimedOut()) {
			Swift::sleep(std::min(100, timeout));
			eventLoop.runOnce();
		}
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_index(lua_State *L) {
	std::string key(luaL_checkstring(L, 2));
	if (key == "debug") {
		lua_pushboolean(L, debug);
		return 1;
	}
	else if (key == "timeout") {
		lua_pushnumber(L, globalTimeout);
		return 1;
	}
	else {
		return luaL_error(L, "Invalid index");
	}
}

static int sluift_newindex(lua_State *L) {
	std::string key(luaL_checkstring(L, 2));
	if (key == "debug") {
		debug = lua_toboolean(L, 3);
		return 0;
	}
	else if (key == "timeout") {
		globalTimeout = luaL_checknumber(L, 3);
		return 0;
	}
	else {
		return luaL_error(L, "Invalid index");
	}
}

static const luaL_reg sluift_functions[] = {
	{"new_client", sluift_new_client},
	{"jid_to_bare", sluift_jid_to_bare},
	{"jid_node", sluift_jid_node},
	{"jid_domain", sluift_jid_domain},
	{"jid_resource", sluift_jid_resource},
	{"sleep", sluift_sleep},
	{NULL, NULL}
};


/*******************************************************************************
 * Module registration
 ******************************************************************************/

SLUIFT_API int luaopen_sluift(lua_State *L) {
	// Register functions
	luaL_register(L, "sluift", sluift_functions);
	lua_createtable(L, 0, 0);
	lua_pushcclosure(L, sluift_index, 0);
	lua_setfield(L, -2, "__index");
	lua_pushcclosure(L, sluift_newindex, 0);
	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);

	// Register the client metatable
	luaL_newmetatable(L, SLUIFT_CLIENT);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, sluift_client_functions);

	return 1;
}
