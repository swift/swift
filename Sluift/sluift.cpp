/*
 * Copyright (c) 2011-2014 Remko Tronçon
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
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/Lua/FunctionRegistration.h>
#include <Swiften/Base/sleep.h>
#include <Swiften/Base/foreach.h>
#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Parser/PayloadParsers/UnitTest/PayloadsParserTester.h>
#include <Swiften/Serializer/PayloadSerializers/FullPayloadSerializerCollection.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/TLS/Certificate.h>
#include <Swiften/TLS/CertificateFactory.h>
#include <Sluift/LuaElementConvertor.h>
#include <Sluift/Lua/Debug.h>
#include <Swiften/StringCodecs/Base64.h>
#include <Swiften/StringCodecs/Hexify.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Sluift/ITunesInterface.h>

using namespace Swift;

namespace Swift {
	namespace Sluift {
		SluiftGlobals globals;
	}
}

extern "C" const char core_lua[];
extern "C" size_t core_lua_size;

/*******************************************************************************
 * Module functions
 ******************************************************************************/

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, new_client,

		"Creates a new client.\n\nReturns a @{Client} object.\n",

		"jid  The JID to connect as\n"
		"passphrase  The passphrase to use\n",

		""
) {
	Lua::checkString(L, 1);
	JID jid(std::string(Lua::checkString(L, 1)));
	std::string password(Lua::checkString(L, 2));

	SluiftClient** client = reinterpret_cast<SluiftClient**>(lua_newuserdata(L, sizeof(SluiftClient*)));

	lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
	lua_getfield(L, -1, "Client");
	lua_setmetatable(L, -3);
	lua_pop(L, 1);

	*client = new SluiftClient(jid, password, &Sluift::globals.networkFactories, &Sluift::globals.eventLoop);
	return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, sha1,
		"Compute the SHA-1 hash of given data",
		"data  the data to hash",
		""
) {
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

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, sleep,
		"Sleeps for the given time.",
		"milliseconds  the amount of milliseconds to sleep",
		""
) {
	Sluift::globals.eventLoop.runOnce();
	int timeout = Lua::checkIntNumber(L, 1);
	Watchdog watchdog(timeout, Sluift::globals.networkFactories.getTimerFactory());
	while (!watchdog.getTimedOut()) {
		Swift::sleep(boost::numeric_cast<unsigned int>(std::min(100, timeout)));
		Sluift::globals.eventLoop.runOnce();
	}
	return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, new_uuid,
		"Generates a new UUID", "", ""
) {
	lua_pushstring(L, IDGenerator().generateID().c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, from_xml,
		"Convert a raw XML string into a structured representation.",
		"string  the string to convert",
		""
) {
	PayloadsParserTester parser;
	if (!parser.parse(Lua::checkString(L, 1))) {
		throw Lua::Exception("Error in XML");
	}
	return Sluift::globals.elementConvertor.convertToLua(L, parser.getPayload());
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, to_xml,
		"Convert a structured element into XML.",
		"element  the element to convert",
		""
) {
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

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, hexify,
		"Convert binary data into hexadecimal format.",
		"data  the data to convert",
		""
) {
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	size_t len;
	const char* data = lua_tolstring(L, 1, &len);
	lua_pushstring(L, Hexify::hexify(createByteArray(data, len)).c_str());
	return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Sluift, unhexify,
		"Convert hexadecimal data into binary data.",
		"data  the data in hexadecimal format",
		""
) {
	if (!lua_isstring(L, 1)) {
		throw Lua::Exception("Expected string");
	}
	ByteArray result = Hexify::unhexify(lua_tostring(L, 1));
	lua_pushlstring(L, reinterpret_cast<char*>(vecptr(result)), result.size());
	return 1;
}

/*******************************************************************************
 * Crypto functions
 ******************************************************************************/

SLUIFT_LUA_FUNCTION_WITH_HELP(
		Crypto, new_certificate,
		"Creates a new X.509 certificate from DER data.\n",

		"der  the DER-encoded certificate data",

		"") {
	ByteArray certData(Lua::checkByteArray(L, 1));
	Certificate::ref cert(Sluift::globals.tlsFactories.getCertificateFactory()->createCertificateFromDER(certData));
	lua_createtable(L, 0, 0);
	lua_pushstring(L, cert->getSubjectName().c_str());
	lua_setfield(L, -2, "subject_name");
	lua_pushstring(L, Certificate::getSHA1Fingerprint(cert, Sluift::globals.networkFactories.getCryptoProvider()).c_str());
	lua_setfield(L, -2, "sha1_fingerprint");

	Lua::pushStringArray(L, cert->getCommonNames());
	lua_setfield(L, -2, "common_names");

	Lua::pushStringArray(L, cert->getSRVNames());
	lua_setfield(L, -2, "srv_names");

	Lua::pushStringArray(L, cert->getDNSNames());
	lua_setfield(L, -2, "dns_names");

	Lua::pushStringArray(L, cert->getXMPPAddresses());
	lua_setfield(L, -2, "xmpp_addresses");

	Lua::registerTableToString(L, -1);
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
 * iTunes Functions
 ******************************************************************************/

#ifdef HAVE_ITUNES
SLUIFT_LUA_FUNCTION(iTunes, get_current_track) {
	boost::optional<ITunesInterface::Track> track = Sluift::globals.iTunes.getCurrentTrack();
	if (!track) {
		return 0;
	}
	lua_createtable(L, 0, 0);
	lua_pushstring(L, track->artist.c_str());
	lua_setfield(L, -2, "artist");
	lua_pushstring(L, track->name.c_str());
	lua_setfield(L, -2, "name");
	lua_pushstring(L, track->album.c_str());
	lua_setfield(L, -2, "album");
	lua_pushinteger(L, track->trackNumber);
	lua_setfield(L, -2, "track_number");
	lua_pushnumber(L, track->duration);
	lua_setfield(L, -2, "duration");
	lua_pushinteger(L, track->rating);
	lua_setfield(L, -2, "rating");
	Lua::registerTableToString(L, -1);
	Lua::registerTableEquals(L, -1);
	return 1;
}
#endif

/*******************************************************************************
 * Module registration
 ******************************************************************************/

static const luaL_Reg sluift_functions[] = { {NULL, NULL} };

SLUIFT_API int luaopen_sluift(lua_State* L) {
	// Initialize & store the module table
	luaL_register(L, lua_tostring(L, 1), sluift_functions);
	lua_pushinteger(L, -1);
	lua_setfield(L, -2, "timeout");
	lua_pushboolean(L, 0);
	lua_setfield(L, -2, "debug");

	lua_pushvalue(L, -1);
	Sluift::globals.moduleLibIndex = luaL_ref(L, LUA_REGISTRYINDEX);

	// Load core lib code
	if (luaL_loadbuffer(L, core_lua, core_lua_size, "core.lua") != 0) {
		lua_error(L);
	}
	lua_pushvalue(L, -2);
	lua_call(L, 1, 1);
	Sluift::globals.coreLibIndex = luaL_ref(L, LUA_REGISTRYINDEX);

	// Register functions
	Lua::FunctionRegistry::getInstance().addFunctionsToTable(L, "Sluift");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "JID");
	lua_setfield(L, -2, "jid");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "Base64");
	lua_setfield(L, -2, "base64");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "IDN");
	lua_setfield(L, -2, "idn");
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "Crypto");
	lua_setfield(L, -2, "crypto");
#ifdef HAVE_ITUNES
	Lua::FunctionRegistry::getInstance().createFunctionTable(L, "iTunes");
	lua_setfield(L, -2, "itunes");
#endif

	// Register convenience functions
	lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
	std::vector<std::string> coreLibExports = boost::assign::list_of
		("tprint")("disco")("help")("get_help")("copy")("with")("read_file");
	foreach (const std::string& coreLibExport, coreLibExports) {
		lua_getfield(L, -1, coreLibExport.c_str());
		lua_setfield(L, -3, coreLibExport.c_str());
	}
	lua_pop(L, 1);

	// Load client metatable
	lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
	std::vector<std::string> tables = boost::assign::list_of("Client");
	foreach(const std::string& table, tables) {
		lua_getfield(L, -1, table.c_str());
		Lua::FunctionRegistry::getInstance().addFunctionsToTable(L, table);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	// Register documentation for all elements
	foreach (boost::shared_ptr<LuaElementConvertor> convertor, Sluift::globals.elementConvertor.getConvertors()) {
		boost::optional<LuaElementConvertor::Documentation> documentation = convertor->getDocumentation();
		if (documentation) {
			Lua::registerClassHelp(L, documentation->className, documentation->description);
		}
	}

	// Register global documentation
	Lua::registerExtraHelp(L, -1, "sluift");

	return 1;
}
