/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Console.h>

#include <cctype>
#include <iostream>
#include <stdexcept>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>

#include <lua.hpp>

#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/Terminal.h>
#include <Sluift/tokenize.h>

using namespace Swift;

/**
 * This function is called by pcall() when an error happens.
 * Adds the backtrace to the error message.
 */
static int traceback(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 1;
    }
    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return 1;
    }
    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return 1;
    }
    lua_pushvalue(L, 1);
    lua_pushinteger(L, 2);
    lua_call(L, 2, 1);
    return 1;
}


Console::Console(lua_State* L, Terminal* terminal) : L(L), terminal(terminal), previousNumberOfReturnArguments(0) {
    terminal->setCompleter(this);
}

Console::~Console() {
}

void Console::run() {
    while (true) {
        lua_settop(L, 0);
        try {
            if (!readCommand()) {
                return;
            }
            int result = call(L, 0, true);
            if (result != 0) {
                throw std::runtime_error(getErrorMessage());
            }

            // Clear the previous results
            for (int i = 0; i < previousNumberOfReturnArguments; ++i) {
                lua_pushnil(L);
                lua_setglobal(L, ("_" + boost::lexical_cast<std::string>(i+1)).c_str());
            }

            // Store the results
            for (int i = 0; i < lua_gettop(L); ++i) {
                lua_pushvalue(L, i+1);
                lua_setglobal(L, ("_" + boost::lexical_cast<std::string>(i+1)).c_str());
            }
            previousNumberOfReturnArguments = lua_gettop(L);

            // Print results
            if (lua_gettop(L) > 0) {
                lua_getglobal(L, "print");
                lua_insert(L, 1);
                if (lua_pcall(L, lua_gettop(L)-1, 0, 0) != 0) {
                    throw std::runtime_error("Error calling 'print': " + getErrorMessage());
                }
            }
        }
        catch (const std::exception& e) {
            terminal->printError(e.what());
        }
    }

}

int Console::tryLoadCommand(const std::string& originalCommand) {
    std::string command = originalCommand;

    // Replace '=' by 'return' (for compatibility with Lua console)
    if (boost::algorithm::starts_with(command, "=")) {
        command = "return " + command.substr(1);
    }

    std::string commandAsExpression = "return " + command;

    // Try to load the command as an expression
    if (luaL_loadbuffer(L, commandAsExpression.c_str(), commandAsExpression.size(), "=stdin") == 0) {
        return 0;
    }
    lua_pop(L, 1);

    // Try to load the command as a regular command
    return luaL_loadbuffer(L, command.c_str(), command.size(), "=stdin");
}

bool Console::readCommand() {
    boost::optional<std::string> line = terminal->readLine(getPrompt(true));
    if (!line) {
        return false;
    }
    std::string command = *line;
    while (true) {
        int result = tryLoadCommand(command);

        // Check if we need to read more
        if (result == LUA_ERRSYNTAX) {
            std::string errorMessage(lua_tostring(L, -1));
            if (boost::algorithm::ends_with(errorMessage, "'<eof>'")) {
                lua_pop(L, 1);

                // Read another line
                boost::optional<std::string> line = terminal->readLine(getPrompt(false));
                if (!line) {
                    return false;
                }
                command = command + "\n" + *line;
                continue;
            }
        }
        if (!command.empty()) {
            terminal->addToHistory(command);
        }
        if (result != 0) {
            throw std::runtime_error(getErrorMessage());
        }
        return true;
    }
}

std::string Console::getErrorMessage() const {
    if (lua_isnil(L, -1)) {
        return "<null error>";
    }
    const char* errorMessage = lua_tostring(L, -1);
    return errorMessage ? errorMessage : "<error is not a string>";
}

int Console::call(lua_State* L, int numberOfArguments, bool keepResult) {
    // Put traceback function on stack below call
    int tracebackIndex = lua_gettop(L) - numberOfArguments;
    lua_pushcfunction(L, traceback);
    lua_insert(L, tracebackIndex);

    int result = lua_pcall(L, numberOfArguments, keepResult ? LUA_MULTRET : 0, tracebackIndex);

    // Remove traceback
    lua_remove(L, tracebackIndex);

    return result;
}

std::string Console::getPrompt(bool firstLine) const {
    lua_getglobal(L,firstLine ? "_PROMPT" : "_PROMPT2");
    const char* rawPrompt = lua_tostring(L, -1);
    std::string prompt;
    if (rawPrompt) {
        prompt = std::string(rawPrompt);
    }
    else {
        prompt = firstLine ? "> " : ">> ";
    }
    lua_pop(L, 1);
    return prompt;
}

static void addMatchingTableKeys(lua_State* L, const std::string& match, std::vector<std::string>& result) {
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
        const char* rawKey = lua_tostring(L, -2);
        if (rawKey) {
            std::string key(rawKey);
            if (boost::starts_with(key, match) && !(match == "" && boost::starts_with(key, "_"))) {
                result.push_back(key);
            }
        }
    }
}

static void addMatchingTableValues(lua_State* L, const std::string& match, std::vector<std::string>& result) {
    for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
        const char* rawValue = lua_tostring(L, -1);
        if (rawValue) {
            std::string key(rawValue);
            if (boost::starts_with(key, match) && !(match == "" && boost::starts_with(key, "_"))) {
                result.push_back(key);
            }
        }
    }
}

std::vector<std::string> Console::getCompletions(const std::string& input, int start, int end) {
    std::string prefix = input.substr(boost::numeric_cast<size_t>(start), boost::numeric_cast<size_t>(end - start));

    std::vector<std::string> tokens;
    if (end) {
        tokens = Lua::tokenize(input.substr(0, boost::numeric_cast<size_t>(end)));
    }

    // Don't autocomplete strings
    if (!tokens.empty() && ((*tokens.rbegin())[0] == '\'' || (*tokens.rbegin())[0] == '"')) {
        return std::vector<std::string>();
    }

    std::vector<std::string> context;
    for (std::vector<std::string>::reverse_iterator i = tokens.rbegin(); i != tokens.rend(); ++i) {
        if (std::isalpha((*i)[0]) || (*i)[0] == '_') {
            if (i != tokens.rbegin()) {
                context.push_back(*i);
            }
        }
        else if (*i != "." && *i != ":") {
            break;
        }
    }

    // Drill into context
    int top = lua_gettop(L);
    lua_pushglobaltable(L);
    for (std::vector<std::string>::reverse_iterator i = context.rbegin(); i != context.rend(); ++i) {
        if (lua_istable(L, -1) || lua_isuserdata(L, -1)) {
            lua_getfield(L, -1, i->c_str());
            if (!lua_isnil(L, 1)) {
                continue;
            }
        }
        lua_settop(L, top);
        return std::vector<std::string>();
    }

    // Collect all keys from the table
    std::vector<std::string> result;
    if (lua_istable(L, -1)) {
        addMatchingTableKeys(L, prefix, result);
    }

    // Collect all keys from the metatable
    if (lua_getmetatable(L, -1)) {
        lua_getfield(L, -1, "__index");
        if (lua_istable(L, -1)) {
            addMatchingTableKeys(L, prefix, result);
        }
        lua_pop(L, 1);

        lua_getfield(L, -1, "_completions");
        if (lua_isfunction(L, -1)) {
            lua_pushvalue(L, -3);
            if (lua_pcall(L, 1, 1, 0) != 0) {
                throw std::runtime_error("Error calling '_completions': " + getErrorMessage());
            }
        }
        if (lua_istable(L, -1)) {
            addMatchingTableValues(L, prefix, result);
        }
        lua_pop(L, 2);
    }

    lua_settop(L, top);

    return result;
}

