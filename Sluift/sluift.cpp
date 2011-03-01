/*
 * Copyright (c) 2011 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

extern "C" {
	#include "sluift.h"
	#include <lauxlib.h>
}

#include <iostream>
#include <string>
#include <deque>
#include <boost/assign/list_of.hpp>

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/JID/JID.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>
#include <Swiften/Queries/RawRequest.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Base/sleep.h>
#include "Watchdog.h"
#include "SluiftException.h"
#include "ResponseSink.h"
#include "Lua/Value.h"

using namespace Swift;

#define SLUIFT_CLIENT "SluiftClient*"

/*******************************************************************************
 * Forward declarations
 ******************************************************************************/

bool debug = false;

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

		void connect() {
			rosterReceived = false;
			client->connect();
		}

		void waitConnected() {
			while (client->isActive() && !client->isAvailable()) {
				eventLoop.runUntilEvents();
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

		std::string sendQuery(const JID& jid, IQ::Type type, const std::string& data) {
			rawRequestResponse.reset();
			RawRequest::ref request = RawRequest::create(type, jid, data, client->getIQRouter());
			request->onResponse.connect(boost::bind(&SluiftClient::handleRawRequestResponse, this, _1));
			request->send();
			while (!rawRequestResponse) {
				eventLoop.runUntilEvents();
			}
			return *rawRequestResponse;
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

		SoftwareVersion::ref getSoftwareVersion(const JID& jid) {
			ResponseSink<SoftwareVersion> sink;
			GetSoftwareVersionRequest::ref request = GetSoftwareVersionRequest::create(jid, client->getIQRouter());
			request->onResponse.connect(boost::ref(sink));
			request->send();
			while (!sink.hasResponse()) {
				eventLoop.runUntilEvents();
			}
			return sink.getResponsePayload();
		}

		Stanza::ref getNextEvent(int timeout) {
			if (!pendingEvents.empty()) {
				Stanza::ref event = pendingEvents.front();
				pendingEvents.pop_front();
				return event;
			}
			Watchdog watchdog(timeout, networkFactories.getTimerFactory());
			while (!watchdog.getTimedOut() && pendingEvents.empty()) {
				eventLoop.runUntilEvents();
			}
			if (watchdog.getTimedOut()) {
				return Stanza::ref();
			}
			else {
				Stanza::ref event = pendingEvents.front();
				pendingEvents.pop_front();
				return event;
			}
		}

		std::vector<XMPPRosterItem> getRoster() {
			client->requestRoster();
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
		client->connect();
		client->waitConnected();
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_async_connect(lua_State *L) {
	try {
		getClient(L)->connect();
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_wait_connected(lua_State *L) {
	try {
		getClient(L)->waitConnected();
		return 0;
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
		return 0;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_set_version(lua_State *L) {
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
	return 0;
}

static int sluift_client_get_roster(lua_State *L) {
	try {
		SluiftClient* client = getClient(L);
		Lua::Table rosterTable;
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
				("name", boost::make_shared<Lua::Value>(item.getName()))
				("subscription", boost::make_shared<Lua::Value>(subscription))
				("groups", boost::make_shared<Lua::Value>(std::vector<Lua::Value>(item.getGroups().begin(), item.getGroups().end())));
			rosterTable[item.getJID().toString()] = boost::make_shared<Lua::Value>(itemTable);
		}
		pushValue(L, rosterTable);
		return 1;
	}
	catch (const SluiftException& e) {
		return luaL_error(L, e.getReason().c_str());
	}
}

static int sluift_client_get_version(lua_State *L) {
	try {
		SluiftClient* client = getClient(L);
		JID jid(std::string(luaL_checkstring(L, 2)));
		SoftwareVersion::ref version = client->getSoftwareVersion(jid);
		if (version) {
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
	getClient(L)->sendMessage(std::string(luaL_checkstring(L, 2)), luaL_checkstring(L, 3));
	return 0;
}

static int sluift_client_send_presence(lua_State *L) {
	getClient(L)->sendPresence(std::string(luaL_checkstring(L, 2)));
	return 0;
}

static int sluift_client_get(lua_State *L) {
	SluiftClient* client = getClient(L);
	JID jid;
	std::string data;
	if (lua_type(L, 3) != LUA_TNONE) {
		jid = JID(std::string(luaL_checkstring(L, 2)));
		data = std::string(luaL_checkstring(L, 3));
	}
	else {
		data = std::string(luaL_checkstring(L, 2));
	}
	std::string result = client->sendQuery(jid, IQ::Get, data);
	lua_pushstring(L, result.c_str());
	return 1;
}

static int sluift_client_set(lua_State *L) {
	SluiftClient* client = getClient(L);
	JID jid;
	std::string data;
	if (lua_type(L, 3) != LUA_TNONE) {
		jid = JID(std::string(luaL_checkstring(L, 2)));
		data = std::string(luaL_checkstring(L, 3));
	}
	else {
		data = std::string(luaL_checkstring(L, 2));
	}
	std::string result = client->sendQuery(jid, IQ::Set, data);
	lua_pushstring(L, result.c_str());
	return 1;
}

static int sluift_client_send(lua_State *L) {
	getClient(L)->getClient()->sendData(std::string(luaL_checkstring(L, 2)));
	return 0;
}

static int sluift_client_set_options(lua_State* L) {
	SluiftClient* client = getClient(L);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_getfield(L, 2, "compress");
	if (!lua_isnil(L, -1)) {
		client->getClient()->setUseStreamCompression(lua_toboolean(L, -1));
	}
	lua_getfield(L, 2, "tls");
	if (!lua_isnil(L, -1)) {
		bool useTLS = lua_toboolean(L, -1);
		client->getClient()->setUseTLS(useTLS ? Client::UseTLSWhenAvailable : Client::NeverUseTLS);
	}
	return 0;
}

static void pushEvent(lua_State* L, Stanza::ref event) {
	if (Message::ref message = boost::dynamic_pointer_cast<Message>(event)) {
		Lua::Table result = boost::assign::map_list_of
			("type", boost::make_shared<Lua::Value>("message"))
			("from", boost::make_shared<Lua::Value>(message->getFrom().toString()))
			("body", boost::make_shared<Lua::Value>(message->getBody()));
		Lua::pushValue(L, result);
	}
	else if (Presence::ref presence = boost::dynamic_pointer_cast<Presence>(event)) {
		Lua::Table result = boost::assign::map_list_of
			("type", boost::make_shared<Lua::Value>("presence"))
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
	{"get_roster", sluift_client_get_roster},
	{"get_version", sluift_client_get_version},
	{"set_options", sluift_client_set_options},
	{"for_event", sluift_client_for_event},
	{"get_next_event", sluift_client_get_next_event},
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
	luaL_checkstring(L, 2);
	if (std::string(lua_tostring(L, 2)) == "debug") {
		lua_pushboolean(L, debug);
		return 1;
	}
	else {
		return luaL_error(L, "Invalid index");
	}
}

static int sluift_newindex(lua_State *L) {
	luaL_checkstring(L, 2);
	if (std::string(lua_tostring(L, 2)) == "debug") {
		debug = lua_toboolean(L, 3);
		return 0;
	}
	else {
		return luaL_error(L, "Invalid index");
	}
}

static const luaL_reg sluift_functions[] = {
	{"new_client", sluift_new_client},
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
