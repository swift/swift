/*
 * Copyright (c) 2011-2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Sluift/sluift.h>

#include <lua.hpp>

#include <string>
#include <boost/bind.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/assign/list_of.hpp>

#include "Watchdog.h"
#include <Sluift/Lua/Check.h>
#include <Sluift/SluiftClient.h>
#include <Sluift/globals.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/Lua/FunctionRegistration.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Sluift/Lua/Debug.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>

using namespace Swift;

namespace Swift {
	namespace Sluift {
		SluiftGlobals globals;
	}
}

extern "C" const char core_lua[];

/*******************************************************************************
 * Module functions
 ******************************************************************************/

SLUIFT_LUA_FUNCTION(Sluift, new_client) {
	Lua::checkString(L, 1);
	JID jid(std::string(Lua::checkString(L, 1)));
	std::string password(Lua::checkString(L, 2));

	SluiftClient** client = reinterpret_cast<SluiftClient**>(lua_newuserdata(L, sizeof(SluiftClient*)));
	luaL_getmetatable(L, Lua::FunctionRegistry::getMetaTableNameForType("Client").c_str());
	lua_setmetatable(L, -2);

	*client = new SluiftClient(jid, password, &Sluift::globals.networkFactories, &Sluift::globals.eventLoop, &Sluift::globals);
	return 1;
}

SLUIFT_LUA_FUNCTION(Sluift, sha1) {
	static boost::shared_ptr<CryptoProvider> crypto(PlatformCryptoProvider::create());
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	size_t len;
	const char* data = lua_tolstring(L, 1, &len);
	ByteArray result = crypto->getSHA1Hash(createByteArray(data, len));
	lua_pushlstring(L, reinterpret_cast<char*>(vecptr(result)), result.size());
	return 1;
}

SLUIFT_LUA_FUNCTION(Sluift, sleep) {
	Sluift::globals.eventLoop.runOnce();
	int timeout = Lua::checkIntNumber(L, 1);
	Watchdog watchdog(timeout, Sluift::globals.networkFactories.getTimerFactory());
	while (!watchdog.getTimedOut()) {
		Swift::sleep(boost::numeric_cast<unsigned int>(std::min(100, timeout)));
		Sluift::globals.eventLoop.runOnce();
	}
	return 0;
}

SLUIFT_LUA_FUNCTION(Sluift, new_uuid) {
	lua_pushstring(L, IDGenerator().generateID().c_str());
	return 1;
}

static int sluift_index(lua_State* L) {
	try {
		std::string key(Lua::checkString(L, 2));
		if (key == "debug") {
			lua_pushboolean(L, Sluift::globals.debug);
			return 1;
		}
		else if (key == "timeout") {
			lua_pushnumber(L, Sluift::globals.timeout);
			return 1;
		}
		throw Lua::Exception("Unknown property");
	}
	catch (const std::exception& e) {
		return luaL_error(L, e.what());
	}
}


static int sluift_newindex(lua_State* L) {
	try {
		std::string key(Lua::checkString(L, 2));
		if (key == "debug") {
			Sluift::globals.debug = lua_toboolean(L, 3);
		}
		else if (key == "timeout") {
			Sluift::globals.timeout = Lua::checkIntNumber(L, 3);
		}
		return 0;
	}
	catch (const std::exception& e) {
		return luaL_error(L, e.what());
	}
}

SLUIFT_LUA_FUNCTION(Sluift, from_xml) {
	PayloadsParserTester parser;
	if (!parser.parse(Lua::checkString(L, 1))) {
		throw Lua::Exception("Error in XML");
	}
	return Sluift::globals.elementConvertor.convertToLua(L, parser.getPayload());
}

SLUIFT_LUA_FUNCTION(Sluift, to_xml) {
	static FullPayloadSerializerCollection serializers;
	boost::shared_ptr<Payload> payload = Sluift::globals.elementConvertor.convertFromLua(L, 1);
	if (!payload) {
		throw Lua::Exception("Unrecognized XML");
	}
	PayloadSerializer* serializer = serializers.getPayloadSerializer(payload);
	if (!payload) {
		throw Lua::Exception("Unrecognized XML");
	}
	lua_pushstring(L, serializer->serialize(payload).c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(Sluift, hexify) {
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	size_t len;
	const char* data = lua_tolstring(L, 1, &len);
	lua_pushstring(L, Hexify::hexify(createByteArray(data, len)).c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(Sluift, unhexify) {
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	ByteArray result = Hexify::unhexify(lua_tostring(L, 1));
	lua_pushlstring(L, reinterpret_cast<char*>(vecptr(result)), result.size());
	return 1;
}


/*******************************************************************************
 * JID Functions
 ******************************************************************************/

SLUIFT_LUA_FUNCTION(JID, to_bare) {
	JID jid(std::string(Lua::checkString(L, 1)));
	lua_pushstring(L, jid.toBare().toString().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(JID, node) {
	JID jid(std::string(Lua::checkString(L, 1)));
	lua_pushstring(L, jid.getNode().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(JID, domain) {
	JID jid(std::string(Lua::checkString(L, 1)));
	lua_pushstring(L, jid.getDomain().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(JID, resource) {
	JID jid(std::string(Lua::checkString(L, 1)));
	lua_pushstring(L, jid.getResource().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(JID, escape_node) {
	lua_pushstring(L, JID::getEscapedNode(Lua::checkString(L, 1)).c_str());
	return 1;
}

/*******************************************************************************
 * Base64 Functions
 ******************************************************************************/

SLUIFT_LUA_FUNCTION(Base64, encode) {
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	size_t len;
	const char* data = lua_tolstring(L, 1, &len);
	lua_pushstring(L, Base64::encode(createByteArray(data, len)).c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(Base64, decode) {
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	ByteArray result = Base64::decode(lua_tostring(L, 1));
	lua_pushlstring(L, reinterpret_cast<char*>(vecptr(result)), result.size());
	return 1;
}

/*******************************************************************************
 * IDN Functions
 ******************************************************************************/

SLUIFT_LUA_FUNCTION(IDN, encode) {
	IDNConverter* converter = Sluift::globals.networkFactories.getIDNConverter();
	lua_pushstring(L, converter->getIDNAEncoded(Lua::checkString(L, 1)).c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION(IDN, stringprep) {
	IDNConverter* converter = Sluift::globals.networkFactories.getIDNConverter();
	IDNConverter::StringPrepProfile profile;
	std::string profileString = Lua::checkString(L, 2);
	if (profileString == "nameprep") {
		profile = IDNConverter::NamePrep;
	}
	else if (profileString == "xmpp_nodeprep") {
		profile = IDNConverter::XMPPNodePrep;
	}
	else if (profileString == "xmpp_resourceprep") {
		profile = IDNConverter::XMPPResourcePrep;
	}
	else if (profileString == "saslprep") {
		profile = IDNConverter::SASLPrep;
	}
	else {
		throw Lua::Exception("Invalid profile");
	}
	try {
		lua_pushstring(L, converter->getStringPrepared(Lua::checkString(L, 1), profile).c_str());
	}
	catch (const std::exception&) {
		throw Lua::Exception("Error");
	}
	return 1;
}


/*******************************************************************************
 * Module registration
 ******************************************************************************/

static const luaL_Reg sluift_functions[] = { {NULL, NULL} };

SLUIFT_API int luaopen_sluift(lua_State* L) {
	// Initialize globals
	Sluift::globals.debug = false;
	Sluift::globals.timeout = -1;

	luaL_register(L, lua_tostring(L, 1), sluift_functions);

	// Load core lib code
	if (luaL_loadbuffer(L, core_lua, strlen(core_lua), "core.lua") != 0) {
		lua_error(L);
	}
	lua_call(L, 0, 1);
	Sluift::globals.coreLibIndex = luaL_ref(L, LUA_REGISTRYINDEX);

	// Register functions
	Lua::FunctionRegistry::getInstance().addFunctionsToTable(L, "Sluift");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "JID");
	lua_setfield(L, -2, "jid");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "Base64");
	lua_setfield(L, -2, "base64");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "IDN");
	lua_setfield(L, -2, "idn");

	// Register convenience functions
	lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
	std::vector<std::string> coreLibExports = boost::assign::list_of
		("tprint")("disco");
	foreach (const std::string& coreLibExport, coreLibExports) {
		lua_getfield(L, -1, coreLibExport.c_str());
		lua_setfield(L, -3, coreLibExport.c_str());
	}
	lua_pop(L, 1);

	// Set read only
	lua_createtable(L, 0, 0);
	lua_pushcclosure(L, sluift_index, 0);
	lua_setfield(L, -2, "__index");
	lua_pushcclosure(L, sluift_newindex, 0);
	lua_setfield(L, -2, "__newindex");
	lua_setmetatable(L, -2);
	
	// Load client metatable
	std::vector<std::string> tables = boost::assign::list_of("Client");
	foreach (const std::string& table, tables) {
		Lua::FunctionRegistry::getInstance().registerTypeMetaTable(L, table);
		luaL_getmetatable(L, Lua::FunctionRegistry::getMetaTableNameForType(table).c_str());
		lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
		lua_getfield(L, -1, table.c_str());
		if (!lua_isnil(L, -1)) {
			for (lua_pushnil(L); lua_next(L, -2); ) {
				lua_pushvalue(L, -2);
				lua_pushvalue(L, -2);
				lua_settable(L, -7);
				lua_pop(L, 1);
			}
		}
		lua_pop(L, 2);
	}
	return 1;
}
