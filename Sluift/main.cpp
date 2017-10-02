/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <iostream>
#include <string>
#include <vector>

#include <boost/assign/list_of.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/version.hpp>

#include <lua.hpp>

#include <Swiften/Base/Platform.h>

#include <Sluift/globals.h>
#include <Sluift/Console.h>
#include <Sluift/StandardTerminal.h>
#include <Sluift/sluift.h>
#include <Sluift/Lua/LuaUtils.h>
#ifdef HAVE_EDITLINE
#include <Sluift/EditlineTerminal.h>
#endif
#include <Sluift/Version.h>

using namespace Swift;

#ifdef SWIFTEN_PLATFORM_WINDOWS
#define EXIT_KEY "Z"
#else
#define EXIT_KEY "D"
#endif

static const std::string SLUIFT_WELCOME_STRING(
        "== Sluift XMPP Console (" SLUIFT_VERSION_STRING ")\nPress Ctrl-" EXIT_KEY " to exit. Type help() for help.");

static const luaL_Reg defaultLibraries[] = {
    {"", luaopen_base},
    {LUA_LOADLIBNAME, luaopen_package},
    {LUA_TABLIBNAME, luaopen_table},
    {LUA_IOLIBNAME, luaopen_io},
    {LUA_OSLIBNAME, luaopen_os},
    {LUA_STRLIBNAME, luaopen_string},
    {LUA_MATHLIBNAME, luaopen_math},
    {LUA_DBLIBNAME, luaopen_debug},
    {"sluift", luaopen_sluift},
    {nullptr, nullptr}
};

static void handleInterruptSignal(int) {
    Sluift::globals.interruptRequested = 1;
}

static void checkResult(lua_State* L, int result) {
    if (result && !lua_isnil(L, -1)) {
        const char* errorMessage = lua_tostring(L, -1);
        throw std::runtime_error(errorMessage ? errorMessage : "Unknown error");
    }
}

static void initialize(lua_State* L) {
    lua_gc(L, LUA_GCSTOP, 0);
    for (const luaL_Reg* lib = defaultLibraries; lib->func; lib++) {
#if LUA_VERSION_NUM >= 502
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);
#else
        lua_pushcfunction(L, lib->func);
        lua_pushstring(L, lib->name);
        lua_call(L, 1, 0);
#endif
    }
    lua_gc(L, LUA_GCRESTART, 0);
}

static void runScript(lua_State* L, const std::string& script, const std::vector<std::string>& scriptArguments) {
    // Create arguments table
    lua_createtable(L, boost::numeric_cast<int>(scriptArguments.size()), 0);
    for (size_t i = 0; i < scriptArguments.size(); ++i) {
        lua_pushstring(L, scriptArguments[i].c_str());
        lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
    }
    lua_setglobal(L, "arg");

    // Load file
    checkResult(L, luaL_loadfile(L, script.c_str()));
    for (const auto& scriptArgument : scriptArguments) {
        lua_pushstring(L, scriptArgument.c_str());
    }
    checkResult(L, Console::call(L, boost::numeric_cast<int>(scriptArguments.size()), false));
}

int main(int argc, char* argv[]) {
    // Parse program options
    boost::program_options::options_description visibleOptions("Options");
    visibleOptions.add_options()
        ("help,h", "Display this help message")
        ("version,v", "Display version information")
        ("interactive,i", "Enter interactive mode after executing script")
        ;
    boost::program_options::options_description hiddenOptions("Hidden Options");
    hiddenOptions.add_options()
        ("script", boost::program_options::value< std::string >(), "Script to be executed")
        ("script-arguments", boost::program_options::value< std::vector<std::string> >(), "Script arguments")
        ;
    boost::program_options::options_description options("All Options");
    options.add(visibleOptions).add(hiddenOptions);

    boost::program_options::positional_options_description positional_options;
    positional_options.add("script", 1).add("script-arguments", -1);

    boost::program_options::variables_map arguments;
    try {
        boost::program_options::store(
                boost::program_options::command_line_parser(argc, argv)
                    .options(options)
                    .positional(positional_options).run(), arguments);
    }
    catch (const boost::program_options::unknown_option& option) {
#if BOOST_VERSION >= 104200
        std::cout << "Ignoring unknown option " << option.get_option_name() << " but continuing." <<  std::endl;
#else
        std::cout << "Error: " << option.what() << " (continuing)" <<  std::endl;
#endif
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }
    boost::program_options::notify(arguments);

    // Help & version
    if (arguments.count("help")) {
        std::cout << visibleOptions << "\n";
        return 0;
    }
    else if (arguments.count("version")) {
        std::cout << SLUIFT_VERSION_STRING;
        return 0;
    }

    lua_State* L = luaL_newstate();
    initialize(L);
    try {
        // Run script
        if (arguments.count("script")) {
            std::vector<std::string> scriptArguments;
            if (arguments.count("script-arguments")) {
                scriptArguments = arguments["script-arguments"].as< std::vector<std::string> >();
            }
            runScript(L, arguments["script"].as<std::string>(), scriptArguments);
        }

        // Run console
        if (arguments.count("interactive") || arguments.count("script") == 0) {
            // Set up signal handler
            signal(SIGINT, handleInterruptSignal);

            // Import some useful functions into the global namespace
            lua_getglobal(L, "sluift");
            std::vector<std::string> globalImports = boost::assign::list_of
                ("help")("with");
            for (const auto& globalImport : globalImports) {
                lua_getfield(L, -1, globalImport.c_str());
                lua_setglobal(L, globalImport.c_str());
            }
            lua_pop(L, 1);

            std::cout << SLUIFT_WELCOME_STRING << std::endl;
#ifdef HAVE_EDITLINE
            EditlineTerminal& terminal = EditlineTerminal::getInstance();
#else
            StandardTerminal terminal;
#endif
            Console console(L, &terminal);
            console.run();
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        lua_close(L);
        return -1;
    }
    lua_close(L);
    return 0;
}
