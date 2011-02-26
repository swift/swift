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

#include <Swiften/Client/Client.h>
#include <Swiften/Client/ClientXMLTracer.h>
#include <Swiften/JID/JID.h>
#include <Swiften/EventLoop/SimpleEventLoop.h>
#include <Swiften/Network/BoostNetworkFactories.h>
#include <Swiften/Network/TimerFactory.h>
#include <Swiften/Network/Timer.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>
#include <Swiften/Roster/XMPPRoster.h>

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

class Watchdog {
	public:
		Watchdog(int timeout) : timedOut(false) {
			if (timeout > 0) {
				timer = networkFactories.getTimerFactory()->createTimer(timeout);
				timer->start();
				timer->onTick.connect(boost::bind(&Watchdog::handleTimerTick, this));
			}
			else if (timeout == 0) {
				timedOut = true;
			}
		}

		~Watchdog() {
			if (timer) {
				timer->stop();
			}
		}

		bool getTimedOut() const {
			return timedOut;
		}
	
	private:
		void handleTimerTick() {
			timedOut = true;
		}
	
	private:
		Timer::ref timer;
		bool timedOut;
};

class SluiftException {
	public:
		SluiftException(const std::string& reason) : reason(reason) {
		}

		const std::string& getReason() const {
			return reason;
		}
	
	private:
		std::string reason;
};

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

		void disconnect() {
			client->disconnect();
			while (client->isActive()) {
				eventLoop.runUntilEvents();
			}
		}

		void setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os) {
			client->setSoftwareVersion(name, version, os);
		}

		boost::optional<SoftwareVersion> getSoftwareVersion(const JID& jid) {
			GetSoftwareVersionRequest::ref request = GetSoftwareVersionRequest::create(jid, client->getIQRouter());
			request->onResponse.connect(boost::bind(&SluiftClient::handleSoftwareVersionResponse, this, _1, _2));
			softwareVersion.reset();
			error.reset();
			request->send();
			while (!softwareVersion && !error) {
				eventLoop.runUntilEvents();
			}
			return softwareVersion;
		}

		Stanza::ref getNextEvent(int timeout) {
			eventLoop.runOnce();
			if (!pendingEvents.empty()) {
				Stanza::ref event = pendingEvents.front();
				pendingEvents.pop_front();
				return event;
			}
			Watchdog watchdog(timeout);
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

		void handleDisconnected(const boost::optional<ClientError>& error) {
			if (error) {
				std::string reason("Disconnected: ");
				switch(error->getType()) {
					case ClientError::UnknownError: reason += "Unknown Error"; break;
					case ClientError::DomainNameResolveError: reason += "Unable to find server"; break;
					case ClientError::ConnectionError: reason += "Error connecting to server"; break;
					case ClientError::ConnectionReadError: reason += "Error while receiving server data"; break;
					case ClientError::ConnectionWriteError: reason += "Error while sending data to the server"; break;
					case ClientError::XMLError: reason += "Error parsing server data"; break;
					case ClientError::AuthenticationFailedError: reason += "Login/password invalid"; break;
					case ClientError::CompressionFailedError: reason += "Error while compressing stream"; break;
					case ClientError::ServerVerificationFailedError: reason += "Server verification failed"; break;
					case ClientError::NoSupportedAuthMechanismsError: reason += "Authentication mechanisms not supported"; break;
					case ClientError::UnexpectedElementError: reason += "Unexpected response"; break;
					case ClientError::ResourceBindError: reason += "Error binding resource"; break;
					case ClientError::SessionStartError: reason += "Error starting session"; break;
					case ClientError::StreamError: reason += "Stream error"; break;
					case ClientError::TLSError: reason += "Encryption error"; break;
					case ClientError::ClientCertificateLoadError: reason += "Error loading certificate (Invalid password?)"; break;
					case ClientError::ClientCertificateError: reason += "Certificate not authorized"; break;
					case ClientError::UnknownCertificateError: reason += "Unknown certificate"; break;
					case ClientError::CertificateExpiredError: reason += "Certificate has expired"; break;
					case ClientError::CertificateNotYetValidError: reason += "Certificate is not yet valid"; break;
					case ClientError::CertificateSelfSignedError: reason += "Certificate is self-signed"; break;
					case ClientError::CertificateRejectedError: reason += "Certificate has been rejected"; break;
					case ClientError::CertificateUntrustedError: reason += "Certificate is not trusted"; break;
					case ClientError::InvalidCertificatePurposeError: reason += "Certificate cannot be used for encrypting your connection"; break;
					case ClientError::CertificatePathLengthExceededError: reason += "Certificate path length constraint exceeded"; break;
					case ClientError::InvalidCertificateSignatureError: reason += "Invalid certificate signature"; break;
					case ClientError::InvalidCAError: reason += "Invalid Certificate Authority"; break;
					case ClientError::InvalidServerIdentityError: reason += "Certificate does not match the host identity"; break;
				}
				throw SluiftException(reason);
			}
		}
	
	private:
		Client* client;
		ClientXMLTracer* tracer;
		bool rosterReceived;
		boost::optional<SoftwareVersion> softwareVersion;
		ErrorPayload::ref error;
		std::deque<Stanza::ref> pendingEvents;
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
		std::vector<XMPPRosterItem> items = client->getRoster();

		lua_createtable(L, 0, items.size());
		foreach(const XMPPRosterItem& item, items) {
			lua_createtable(L, 0, 3);

			lua_pushstring(L, item.getName().c_str());
			lua_setfield(L, -2, "name");

			std::string subscription;
			switch(item.getSubscription()) {
				case RosterItemPayload::None: subscription = "none"; break;
				case RosterItemPayload::To: subscription = "to"; break;
				case RosterItemPayload::From: subscription = "from"; break;
				case RosterItemPayload::Both: subscription = "both"; break;
				case RosterItemPayload::Remove: subscription = "remove"; break;
			}
			lua_pushstring(L, subscription.c_str());
			lua_setfield(L, -2, "subscription");

			std::vector<std::string> groups = item.getGroups();
			lua_createtable(L, groups.size(), 0);
			for (size_t i = 0; i < groups.size(); ++i) {
				lua_pushstring(L, groups[i].c_str());
				lua_rawseti(L, -2, i + 1);
			}
			lua_setfield(L, -2, "groups");

			lua_setfield(L, -2, item.getJID().toString().c_str());
		}
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

static int sluift_client_set_options(lua_State* L) {
	SluiftClient* client = getClient(L);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_getfield(L, 2, "compress");
	if (!lua_isnil(L, -1)) {
		client->getClient()->setUseStreamCompression(lua_toboolean(L, -1));
	}
	lua_pop(L, -1);
	return 0;
}

static int sluift_client_for_event(lua_State *L) {
	try {
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
				if (Message::ref message = boost::dynamic_pointer_cast<Message>(event)) {
					lua_createtable(L, 0, 3);
					lua_pushliteral(L, "message");
					lua_setfield(L, -2, "type");
					lua_pushstring(L, message->getFrom().toString().c_str());
					lua_setfield(L, -2, "from");
					lua_pushstring(L, message->getBody().c_str());
					lua_setfield(L, -2, "body");
				}
				else if (Presence::ref presence = boost::dynamic_pointer_cast<Presence>(event)) {
					lua_createtable(L, 0, 3);
					lua_pushliteral(L, "presence");
					lua_setfield(L, -2, "type");
					lua_pushstring(L, presence->getFrom().toString().c_str());
					lua_setfield(L, -2, "from");
					lua_pushstring(L, presence->getStatus().c_str());
					lua_setfield(L, -2, "status");
				}
				else {
					assert(false);
					lua_pushnil(L);
				}
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
	{"set_version", sluift_client_set_version},
	{"get_roster", sluift_client_get_roster},
	{"get_version", sluift_client_get_version},
	{"set_options", sluift_client_set_options},
	{"for_event", sluift_client_for_event},
	{"__gc", sluift_client_gc},
	{NULL, NULL}
};

/*******************************************************************************
 * Module functions
 ******************************************************************************/

static int sluift_new_client(lua_State *L) {
	JID jid(std::string(luaL_checkstring(L, 1)));
	std::string password(luaL_checkstring(L, 2));

	SluiftClient** client = reinterpret_cast<SluiftClient**>(lua_newuserdata(L, sizeof(SluiftClient*)));
	luaL_getmetatable(L, SLUIFT_CLIENT);
	lua_setmetatable(L, -2);

	*client = new SluiftClient(jid, password);
	return 1;
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
