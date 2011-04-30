/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <string>
#include <vector>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <boost/filesystem/fstream.hpp>
#include <boost/noncopyable.hpp>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>
#include <Swiften/Network/Timer.h>

#include <Swiftob/Commands.h>
#include <Swiftob/Storage.h>

using namespace Swift;
/**
 * Yes, there's an odd naming scheme going on here for methods.
 * normalCamelCase methods are methods called from C++
 * lower_case_methods are exposed to Lua through wrappers and
 * l_lower_case_functions are functions directly exposed (often calling the lower_case_methods).
 */
class LuaCommands {
	public:
		class LuaCommand : public Commands::Command, boost::noncopyable {
			public:
				/** Takes ownership of lua and storage.*/
				LuaCommand(Commands::RoleList allowedBy, const std::string& description, lua_State* lua, Storage* storage) : Command(allowedBy, description), lua_(lua), storage_(storage) {
				}

				virtual ~LuaCommand() {
					lua_close(lua_);
					delete storage_;
				};

			private:
				lua_State* lua_;
				Storage* storage_;
		};

		class Callbacks {
			public:
				Callbacks(int success, int failure, int timeout) : success(success), failure(failure), timeout(timeout) {};
				int success;
				int failure;
				int timeout;
				void erase(lua_State *L) {
					lua_pushnil(L);
					lua_rawseti(L, LUA_REGISTRYINDEX, success);
					lua_pushnil(L);
					lua_rawseti(L, LUA_REGISTRYINDEX, failure);
					lua_pushnil(L);
					lua_rawseti(L, LUA_REGISTRYINDEX, timeout);
				}
		};

		LuaCommands(Commands* commands, const std::string& path, Client* client, TimerFactory* timerFactory, MUCs* mucs);
		/* Public but isn't really part of the API */
		void handleLuaCommand(int callbackIndex, lua_State* L, const std::string& command, const std::string& params, Message::ref message);
		Commands* getCommands() {return commands_;}
		int get_software_version(lua_State *L);
		int muc_input_to_jid(lua_State *L);
		int store_setting(lua_State *L);
		int get_setting(lua_State *L);
		static LuaCommands* commandsFromLua(lua_State *L);
		static Storage* storageFromLua(lua_State *L);
	private:
		void registerCommands();
		void loadScript(boost::filesystem::path filePath);
		void messageOntoStack(Message::ref message, lua_State* L);
		void handleSoftwareVersionResponse(boost::shared_ptr<SoftwareVersion> version, ErrorPayload::ref error, bool timeout, GetSoftwareVersionRequest::ref request, Timer::ref timer, lua_State* L, Callbacks callbacks);
	private:
		std::string path_;
		boost::filesystem::path scriptsPath_;
		Commands* commands_;
		MUCs* mucs_;
		Client* client_;
		TimerFactory* timerFactory_;
};


