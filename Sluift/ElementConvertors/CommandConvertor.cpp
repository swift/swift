/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/CommandConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/Value.h>
#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

static Command::Action convertActionFromString(const std::string& action) {
    if (action == "cancel") { return Command::Cancel; }
    else if (action == "execute") { return Command::Execute; }
    else if (action == "complete") { return Command::Complete; }
    else if (action == "prev") { return Command::Prev; }
    else if (action == "next") { return Command::Next; }
    return Command::NoAction;
}

static std::string convertActionToString(Command::Action action) {
    switch (action) {
        case Command::Cancel: return "cancel";
        case Command::Execute: return "execute";
        case Command::Complete: return "complete";
        case Command::Prev: return "prev";
        case Command::Next: return "next";
        case Command::NoAction: assert(false); return "";
    }
    assert(false);
    return "";
}

CommandConvertor::CommandConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<Command>("command"),
        convertors(convertors) {
}

CommandConvertor::~CommandConvertor() {
}

std::shared_ptr<Command> CommandConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<Command> result = std::make_shared<Command>();

    lua_getfield(L, -1, "node");
    if (!lua_isnil(L, -1)) {
        result->setNode(std::string(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "session_id");
    if (!lua_isnil(L, -1)) {
        result->setSessionID(std::string(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "status");
    if (!lua_isnil(L, -1)) {
        std::string statusText = Lua::checkString(L, -1);
        Command::Status status = Command::NoStatus;
        if (statusText == "executing") { status = Command::Executing; }
        else if (statusText == "completed") { status = Command::Completed; }
        else if (statusText == "canceled") { status = Command::Canceled; }
        result->setStatus(status);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "action");
    if (!lua_isnil(L, -1)) {
        result->setAction(convertActionFromString(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "execute_action");
    if (!lua_isnil(L, -1)) {
        result->setExecuteAction(convertActionFromString(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "available_actions");
    if (!lua_isnil(L, -1)) {
        Lua::checkType(L, -1, LUA_TTABLE);
        for (lua_pushnil(L); lua_next(L, -2) != 0; ) {
            result->addAvailableAction(convertActionFromString(Lua::checkString(L, -1)));
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "notes");
    if (!lua_isnil(L, -1)) {
        Lua::checkType(L, -1, LUA_TTABLE);
        for (lua_pushnil(L); lua_next(L, -2) != 0; ) {
            Lua::checkType(L, -1, LUA_TTABLE);
            std::string note;
            lua_getfield(L, -1, "note");
            if (!lua_isnil(L, -1)) {
                note = Lua::checkString(L, -1);
            }
            lua_pop(L, 1);

            Command::Note::Type noteType = Command::Note::Info;
            lua_getfield(L, -1, "type");
            if (!lua_isnil(L, -1)) {
                std::string type = Lua::checkString(L, -1);
                if (type == "info") { noteType = Command::Note::Info; }
                else if (type == "warn") { noteType = Command::Note::Warn; }
                else if (type == "error") { noteType = Command::Note::Error; }
            }
            lua_pop(L, 1);

            result->addNote(Command::Note(note, noteType));
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "form");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<Form> form = std::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
            result->setForm(form);
        }
    }
    lua_pop(L, 1);

    return result;
}

void CommandConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Command> payload) {
    Lua::Table result;
    if (!payload->getNode().empty()) {
        result["node"] = Lua::valueRef(payload->getNode());
    }
    if (!payload->getSessionID().empty()) {
        result["session_id"] = Lua::valueRef(payload->getSessionID());
    }
    switch (payload->getStatus()) {
        case Command::Executing: result["status"] = Lua::valueRef("executing"); break;
        case Command::Completed: result["status"] = Lua::valueRef("completed"); break;
        case Command::Canceled: result["status"] = Lua::valueRef("canceled"); break;
        case Command::NoStatus: break;
    }

    if (!payload->getNotes().empty()) {
        std::vector<Lua::Value> notes;
        for (const auto& note : payload->getNotes()) {
            Lua::Table noteTable;
            if (!note.note.empty()) {
                noteTable["note"] = Lua::valueRef(note.note);
            }
            switch (note.type) {
                case Command::Note::Info: noteTable["type"] = Lua::valueRef("info"); break;
                case Command::Note::Warn: noteTable["type"] = Lua::valueRef("warn"); break;
                case Command::Note::Error: noteTable["type"] = Lua::valueRef("error"); break;
            }
            notes.push_back(noteTable);
        }
        result["notes"] = Lua::valueRef(notes);
    }

    if (payload->getAction() != Command::NoAction) {
        result["action"] = Lua::valueRef(convertActionToString(payload->getAction()));
    }

    if (payload->getExecuteAction() != Command::NoAction) {
        result["execute_action"] = Lua::valueRef(convertActionToString(payload->getAction()));
    }

    if (!payload->getAvailableActions().empty()) {
        std::vector<Lua::Value> availableActions;
        for (const auto& action : payload->getAvailableActions()) {
            if (action != Command::NoAction) {
                availableActions.push_back(convertActionToString(action));
            }
        }
        result["available_actions"] = Lua::valueRef(availableActions);
    }

    Lua::pushValue(L, result);

    if (payload->getForm()) {
        bool result = convertors->convertToLuaUntyped(L, payload->getForm());
        assert(result);
        lua_setfield(L, -2, "form");
    }
}
