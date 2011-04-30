/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiftob/LuaCommands.h>

#include <boost/bind.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

#include <Swiften/Base/foreach.h>
#include <Swiften/Client/Client.h>
#include <Swiften/Network/TimerFactory.h>

#include <Swiftob/Commands.h>

#define LUA_COMMANDS "__Lua_Commands"
#define STORAGE "__Storage"

LuaCommands::LuaCommands(Commands* commands, const std::string& path, Client* client, TimerFactory* timerFactory, MUCs* mucs) : path_(path), scriptsPath_(boost::filesystem::path(path_) / "scripts") {
	commands_ = commands;
	client_ = client;
	timerFactory_ = timerFactory;
	mucs_ = mucs;
	commands_->onReset.connect(boost::bind(&LuaCommands::registerCommands, this));
	registerCommands();
}

void LuaCommands::registerCommands() {
	std::cout << "Trying to load all scripts in " << scriptsPath_ << std::endl;
	if (boost::filesystem::exists(scriptsPath_) && boost::filesystem::is_directory(scriptsPath_)) {
		std::vector<boost::filesystem::path> files;
		copy(boost::filesystem::directory_iterator(scriptsPath_), boost::filesystem::directory_iterator(), std::back_inserter(files));
		foreach (boost::filesystem::path file, files) {
			if (boost::filesystem::is_regular_file(file) && file.extension() == ".lua") {
				loadScript(file);
			}
		}
	}
}

static int l_register_command(lua_State *L) {
	LuaCommands* commands = NULL;
	lua_getfield(L, LUA_REGISTRYINDEX, LUA_COMMANDS);
	commands = static_cast<LuaCommands*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	if (!lua_isfunction(L, 4)) {
		return luaL_error(L, "register_command callback parameter must be a function");
	}
	//luaL_ref callback(lua_to(L, 4));
	lua_pushvalue(L, 4);
	int callbackIndex = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_pop(L, 1);

	if (!lua_isstring(L, 3)) {
		return luaL_error(L, "register_command description parameter must be a string");
	}
	std::string description(lua_tostring(L, 3));
	lua_pop(L, 1);

	if (!lua_isstring(L, 2)) {
		return luaL_error(L, "register_command allowed roles parameter must be a string");
	}
	std::string roleString(lua_tostring(L, 2));
	lua_pop(L, 1);
	Commands::RoleList roleList = Commands::Owner;
	if (roleString == "Owner") {
		roleList = Commands::Owner;
	} else if (roleString == "Anyone") {
		roleList = Commands::Anyone;
	} else {
		return luaL_error(L, "register_command allowed roles parameter has illegal value");
	}
	if (!lua_isstring(L, 1)) {
		return luaL_error(L, "register_command command name parameter must be a string");
	}
	std::string name(lua_tostring(L, 1));
	lua_pop(L, 1);
	std::cout << "Registering lua command '" << name << "' for '" << roleString << "' with callback index " << callbackIndex << std::endl;
	commands->getCommands()->registerCommand(name, roleList, description, boost::bind(&LuaCommands::handleLuaCommand, commands, callbackIndex, L, _1, _2, _3));

	return 0;
}

static std::string luatable_asstring(lua_State *L, const char* key) {
	lua_getfield(L, -1, key);
	const char* valueChars = lua_tostring(L, -1);
	std::string value(valueChars != NULL ? valueChars : "");
	lua_pop(L, 1);
	return value;
}

static int luatable_asint(lua_State *L, const char* key) {
	lua_getfield(L, -1, key);
	int value = lua_tointeger(L, -1);
	lua_pop(L, 1);
	return value;
}

static int luatable_asfunction(lua_State *L, const char* key) {
	lua_getfield(L, -1, key);
	int callbackIndex = luaL_ref(L, LUA_REGISTRYINDEX);
	return callbackIndex;
}

static Message::ref messageFromTable(lua_State *L) {
	Message::ref message(new Message());
	message->setFrom(JID(luatable_asstring(L, "from")));
	message->setBody(luatable_asstring(L, "body"));
	Message::Type type = Message::Normal;
	std::string typeString(luatable_asstring(L, "type"));
	if (typeString == "normal") {
		type = Message::Normal;
	} else if (typeString == "chat") {
		type = Message::Chat;
	} else if (typeString == "groupchat") {
		type = Message::Groupchat;
	} else if (typeString == "error") {
		type = Message::Error;
	} else if (typeString == "headline") {
		type = Message::Headline;
	} else {
		return Message::ref();
	}
	message->setType(type);
	return message;
}

LuaCommands* LuaCommands::commandsFromLua(lua_State *L) {
	LuaCommands* commands = NULL;
	lua_getfield(L, LUA_REGISTRYINDEX, LUA_COMMANDS);
	commands = static_cast<LuaCommands*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	return commands;
}

Storage* LuaCommands::storageFromLua(lua_State *L) {
	Storage* storage = NULL;
	lua_getfield(L, LUA_REGISTRYINDEX, STORAGE);
	storage = static_cast<Storage*>(lua_touserdata(L, -1));
	lua_pop(L, 1);
	return storage;
}

static int l_reply_to(lua_State *L) {
	LuaCommands* commands = LuaCommands::commandsFromLua(L);

	if (!lua_isboolean(L, 3) && lua_gettop(L) > 2) {
		return luaL_error(L, "reply_to parameter 3 must be boolean if present");
	}
	bool outOfMUC = lua_toboolean(L, 3);
	if (lua_gettop(L) == 3) {
		lua_pop(L, 1);
	}

	if (!lua_isstring(L, 2)) {
		return luaL_error(L, "reply_to body parameter must be a string");
	}
	std::string body(lua_tostring(L, 2));
	lua_pop(L, 1);

	if (!lua_istable(L, 1)) {
		return luaL_error(L, "reply_to message parameter must be a table");
	}
	lua_pushvalue(L, 1);
	Message::ref message(messageFromTable(L));
	if (!message) {
		return luaL_error(L, "message parameter invalid");
	}
	commands->getCommands()->replyTo(message, body, outOfMUC);
	lua_pop(L, 1);

	return 0;
}

static int l_muc_input_to_jid(lua_State *L) {
	LuaCommands* commands = LuaCommands::commandsFromLua(L);
	return commands->muc_input_to_jid(L);
}

int LuaCommands::muc_input_to_jid(lua_State *L) {
	if (!lua_isstring(L, 2)) {
		return luaL_error(L, "must pass a string to muc_input_to_jid p2");
	}
	std::string source = lua_tostring(L, 2);
	JID sourceJID(source);
	lua_pop(L, 1);
	if (!lua_isstring(L, 1)) {
		return luaL_error(L, "must pass a string to muc_input_to_jid p1");
	}
	std::string input = lua_tostring(L, 1);
	lua_pop(L, 1);
	JID result(input);
	if (mucs_->contains(sourceJID.toBare())) {
		if (result.isBare() && result.getNode().empty()) {
			if (mucs_->getMUC(sourceJID.toBare())->hasOccupant(input)) {
				result = JID(sourceJID.getNode(), sourceJID.getDomain(), input);
			}
		}
	}

	lua_pushstring(L, result.isValid() ? result.toString().c_str() : "");
	return 1;
}

void LuaCommands::handleSoftwareVersionResponse(boost::shared_ptr<SoftwareVersion> version, ErrorPayload::ref error, bool timeout, GetSoftwareVersionRequest::ref request, Timer::ref timer, lua_State* L, Callbacks callbacks) {
	request->onResponse.disconnect_all_slots();
	timer->onTick.disconnect_all_slots();
	timer->stop();
	int callback = callbacks.failure;
	int stackCount = 0;
	if (timeout) {
		callback = callbacks.timeout;
	} else if (version) {
		callback = callbacks.success;
	}
	lua_rawgeti(L, LUA_REGISTRYINDEX, callback);
	if (error) {
		lua_pushstring(L, error->getText().empty() ? "No error text" : error->getText().c_str());
		stackCount++;
	}
	else if (version) {
		lua_createtable(L, 0, 3);
		lua_pushstring(L, version->getName().c_str());
		lua_setfield(L, -2, "name");
		lua_pushstring(L, version->getVersion().c_str());
		lua_setfield(L, -2, "version");
		lua_pushstring(L, version->getOS().c_str());
		lua_setfield(L, -2, "os");
		stackCount++;
	}
	else {
		lua_pushliteral(L, "Missing payload");
		stackCount++;
	}
	int result = lua_pcall(L, stackCount, 0, 0);
	if (result != 0) {
		std::string error(lua_tostring(L, -1));
		lua_pop(L, 1);
		std::cout << error << std::endl;
		callbacks.erase(L);
		luaL_error(L, error.c_str());
	} else {
		callbacks.erase(L);
	}

}

static int l_get_software_version(lua_State *L) {
	LuaCommands* commands = LuaCommands::commandsFromLua(L);
	return commands->get_software_version(L);
}

int LuaCommands::get_software_version(lua_State *L) {
	if (!lua_istable(L, 1)) {
		return luaL_error(L, "get_software_version requires a table parameter.");
	}
	lua_pushvalue(L, 1);
	JID to(luatable_asstring(L, "to"));
	if (!to.isValid()) {
		return luaL_error(L, "invalid JID.");
	}
	int timeout = luatable_asint(L, "timeout");
	if (timeout == 0) {
		return luaL_error(L, "invalid timeout.");
	}

	int successCallback = luatable_asfunction(L, "success_callback");
	int failureCallback = luatable_asfunction(L, "failure_callback");
	int timeoutCallback = luatable_asfunction(L, "timeout_callback");
	GetSoftwareVersionRequest::ref request = GetSoftwareVersionRequest::create(to, client_->getIQRouter());
	Timer::ref timer = timerFactory_->createTimer(timeout * 1000);
	Callbacks callbacks(successCallback, failureCallback, timeoutCallback);
	request->onResponse.connect(boost::bind(&LuaCommands::handleSoftwareVersionResponse, this, _1, _2, false, request, timer, L, callbacks));
	boost::shared_ptr<SoftwareVersion> fakePayload;
	ErrorPayload::ref fakeError;
	timer->onTick.connect(boost::bind(&LuaCommands::handleSoftwareVersionResponse, this, fakePayload, fakeError, true, request, timer, L, callbacks));
	timer->start();
	request->send();
	return 1;
}

static int l_store_setting(lua_State *L) {
	return LuaCommands::commandsFromLua(L)->store_setting(L);
}

static int l_get_setting(lua_State *L) {
	return LuaCommands::commandsFromLua(L)->get_setting(L);
}

int LuaCommands::store_setting(lua_State *L) {
	if (!lua_isstring(L, 2) || !lua_isstring(L, 1)) {
		return luaL_error(L, "both setting and key must be strings");
	}
	std::string value(lua_tostring(L, 2));
	std::string key(lua_tostring(L, 1));
	lua_pop(L, 2);
	storageFromLua(L)->saveSetting(key, value);
	return 0;
}

int LuaCommands::get_setting(lua_State *L) {
	if (!lua_isstring(L, 1)) {
		return luaL_error(L, "key must be a string");
	}
	std::string key(lua_tostring(L, 1));
	lua_pop(L, 1);
	lua_pushstring(L, storageFromLua(L)->getSetting(key).c_str());
	return 1;

}

void LuaCommands::handleLuaCommand(int callbackIndex, lua_State* L, const std::string& command, const std::string& params, Swift::Message::ref message) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, callbackIndex);
	lua_pushstring(L, command.c_str());
	lua_pushstring(L, params.c_str());
	messageOntoStack(message, L);
	int result = lua_pcall(L, 3, 0, 0);
	if (result != 0) {
		std::string error(lua_tostring(L, -1));
		lua_pop(L, 1);
		error = "Command '" + command + "' failed: " + error;
		std::cout << error << std::endl;
		commands_->replyTo(message, error, false);
	}
}

void LuaCommands::messageOntoStack(Swift::Message::ref message, lua_State* L) {
	lua_createtable(L, 0, 4);
	std::string typeString;
	switch (message->getType()) {
		case Message::Chat : typeString = "chat";break;
		case Message::Groupchat : typeString = "groupchat";break;
		case Message::Normal : typeString = "normal";break;
		case Message::Error : typeString = "error";break;
		case Message::Headline : typeString = "headline";break;
	}
	lua_pushstring(L, typeString.c_str());
	lua_setfield(L, -2, "type");
	lua_pushstring(L, message->getFrom().toString().c_str());
	lua_setfield(L, -2, "from");
	lua_pushstring(L, message->getFrom().toBare().toString().c_str());
	lua_setfield(L, -2, "frombare");
	lua_pushstring(L, message->getTo().toString().c_str());
	lua_setfield(L, -2, "to");
	lua_pushstring(L, message->getBody().c_str());
	lua_setfield(L, -2, "body");
}

void LuaCommands::loadScript(boost::filesystem::path filePath) {
	std::cout << "Trying to load file from " << filePath << std::endl;
	lua_State* lua = lua_open();
	luaL_openlibs(lua);
	lua_pushlightuserdata(lua, this);
	lua_setfield(lua, LUA_REGISTRYINDEX, LUA_COMMANDS);
	std::string filename(filePath.filename());
	filename += ".storage";
	boost::filesystem::path storagePath(boost::filesystem::path(path_) / filename);
	Storage* storage = new Storage(storagePath);
	lua_pushlightuserdata(lua, storage);
	lua_setfield(lua, LUA_REGISTRYINDEX, STORAGE);
	lua_register(lua, "swiftob_register_command", &l_register_command);
	lua_register(lua, "swiftob_reply_to", &l_reply_to);
	lua_register(lua, "swiftob_get_software_version", &l_get_software_version);
	lua_register(lua, "swiftob_muc_input_to_jid", &l_muc_input_to_jid);
	lua_register(lua, "swiftob_store_setting", &l_store_setting);
	lua_register(lua, "swiftob_get_setting", &l_get_setting);
	int fileLoaded = luaL_dofile(lua, filePath.string().c_str());
	if (fileLoaded == 0 ) {
		std::cout << "Loaded" << std::endl;
	} else {
		const char* error = lua_tostring(lua, -1);
		std::cout << "Error: " << error << std::endl;
		lua_pop(lua, -1);
	}
}
