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

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/JID/JID.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>

using namespace Swift;

#define SLUIFT_CLIENT "SluiftClient*"

/*******************************************************************************
 * Forward declarations
 ******************************************************************************/

static int debugRef = 0;

static bool debug(lua_State* L) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, debugRef);
	lua_getfield(L, -1, "debug");
	bool b = lua_toboolean(L, -1);
	lua_pop(L, 2);
	return b;
}

/*******************************************************************************
 * Helper classes
 ******************************************************************************/

SimpleEventLoop eventLoop;
BoostNetworkFactories networkFactories(&eventLoop);

class SluiftClient {
	public:
		SluiftClient(const JID& jid, const std::string& password, bool debug = false) : tracer(NULL) {
			client = new Client(jid, password, &networkFactories);
			client->setAlwaysTrustCertificates();
			if (debug) {
				tracer = new ClientXMLTracer(client);
			}
		}

		~SluiftClient() {
			delete tracer;
			delete client;
		}

		void connect() {
			client->connect();
			while (client->isActive() && !client->isAvailable()) {
				processEvents();
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

		void disconnect() {
			client->disconnect();
			while (client->isActive()) {
				processEvents();
			}
		}

		boost::optional<SoftwareVersion> getSoftwareVersion(const JID& jid) {
			GetSoftwareVersionRequest::ref request = GetSoftwareVersionRequest::create(jid, client->getIQRouter());
			request->onResponse.connect(boost::bind(&SluiftClient::handleSoftwareVersionResponse, this, _1, _2));
			softwareVersion.reset();
			error.reset();
			request->send();
			while (!softwareVersion && !error) {
				processEvents();
			}
			return softwareVersion;
		}

	private:
		void processEvents() {
			eventLoop.runUntilEvents();
		}

		void handleSoftwareVersionResponse(boost::shared_ptr<SoftwareVersion> version, ErrorPayload::ref error) {
			if (error) {
				this->error = error;
			}
			else if (version) {
				this->softwareVersion = *version;
			}
			else {
				this->softwareVersion = SoftwareVersion("", "", "");
			}
		}
	
	private:
		Client* client;
		ClientXMLTracer* tracer;
		boost::optional<SoftwareVersion> softwareVersion;
		ErrorPayload::ref error;
};


/*******************************************************************************
 * Client functions.
 ******************************************************************************/

static inline SluiftClient* getClient(lua_State* L) {
	return *reinterpret_cast<SluiftClient**>(luaL_checkudata(L, 1, SLUIFT_CLIENT));
}

static int sluift_client_is_connected(lua_State *L) {
	lua_pushboolean(L, getClient(L)->isConnected());
	return 1;
}

static int sluift_client_disconnect(lua_State *L) {
	getClient(L)->disconnect();
	return 0;
}


static int sluift_client_get_version(lua_State *L) {
	SluiftClient* client = getClient(L);
	JID jid(std::string(luaL_checkstring(L, 2)));

	boost::optional<SoftwareVersion> version = client->getSoftwareVersion(jid);
	if (version) {
		lua_createtable(L, 0, 3);
		lua_pushstring(L, version->getName().c_str());
		lua_setfield(L, -2, "name");
		lua_pushstring(L, version->getVersion().c_str());
		lua_setfield(L, -2, "version");
		lua_pushstring(L, version->getOS().c_str());
		lua_setfield(L, -2, "os");
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

static int sluift_client_send_message(lua_State *L) {
	getClient(L)->sendMessage(std::string(luaL_checkstring(L, 2)), luaL_checkstring(L, 3));
	return 0;
}

static int sluift_client_gc (lua_State *L) {
	SluiftClient* client = getClient(L);
	delete client;
	return 0;
}


static const luaL_reg sluift_client_functions[] = {
	{"is_connected", sluift_client_is_connected},
	{"disconnect",  sluift_client_disconnect},
	{"send_message", sluift_client_send_message},
	{"get_version", sluift_client_get_version},
	{"__gc", sluift_client_gc},
	{NULL, NULL}
};

/*******************************************************************************
 * Module functions
 ******************************************************************************/

static int sluift_connect(lua_State *L) {
	JID jid(std::string(luaL_checkstring(L, 1)));
	std::string password(luaL_checkstring(L, 2));

	SluiftClient** client = reinterpret_cast<SluiftClient**>(lua_newuserdata(L, sizeof(SluiftClient*)));
	luaL_getmetatable(L, SLUIFT_CLIENT);
	lua_setmetatable(L, -2);

	*client = new SluiftClient(jid, password, debug(L));
	(*client)->connect();
	if (!(*client)->isConnected()) {
		lua_pushnil(L);
	}
	return 1;
}

static const luaL_reg sluift_functions[] = {
	{"connect",   sluift_connect},
	{NULL, NULL}
};


/*******************************************************************************
 * Module registration
 ******************************************************************************/

SLUIFT_API int luaopen_sluift(lua_State *L) {
	// Register functions
	luaL_register(L, "sluift", sluift_functions);
	lua_pushboolean(L, false);
	lua_setfield(L, -2, "debug");
	
	lua_pushvalue(L, -1);
	debugRef = luaL_ref(L, LUA_REGISTRYINDEX);


	// Register the client metatable
	luaL_newmetatable(L, SLUIFT_CLIENT);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, sluift_client_functions);

	return 1;
}
