/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/LuaElementConvertors.h>

#include <memory>

#include <Sluift/ElementConvertors/BodyConvertor.h>
#include <Sluift/ElementConvertors/CarbonsReceivedConvertor.h>
#include <Sluift/ElementConvertors/CarbonsSentConvertor.h>
#include <Sluift/ElementConvertors/CommandConvertor.h>
#include <Sluift/ElementConvertors/DOMElementConvertor.h>
#include <Sluift/ElementConvertors/DefaultElementConvertor.h>
#include <Sluift/ElementConvertors/DelayConvertor.h>
#include <Sluift/ElementConvertors/DiscoInfoConvertor.h>
#include <Sluift/ElementConvertors/DiscoItemsConvertor.h>
#include <Sluift/ElementConvertors/FormConvertor.h>
#include <Sluift/ElementConvertors/ForwardedConvertor.h>
#include <Sluift/ElementConvertors/IQConvertor.h>
#include <Sluift/ElementConvertors/MAMFinConvertor.h>
#include <Sluift/ElementConvertors/MAMQueryConvertor.h>
#include <Sluift/ElementConvertors/MAMResultConvertor.h>
#include <Sluift/ElementConvertors/MessageConvertor.h>
#include <Sluift/ElementConvertors/PresenceConvertor.h>
#include <Sluift/ElementConvertors/PubSubEventConvertor.h>
#include <Sluift/ElementConvertors/RawXMLElementConvertor.h>
#include <Sluift/ElementConvertors/ResultSetConvertor.h>
#include <Sluift/ElementConvertors/SoftwareVersionConvertor.h>
#include <Sluift/ElementConvertors/StatusConvertor.h>
#include <Sluift/ElementConvertors/StatusShowConvertor.h>
#include <Sluift/ElementConvertors/SubjectConvertor.h>
#include <Sluift/ElementConvertors/VCardConvertor.h>
#include <Sluift/ElementConvertors/VCardUpdateConvertor.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/LuaElementConvertor.h>

using namespace Swift;

LuaElementConvertors::LuaElementConvertors() {
    registerConvertors();
    convertors.push_back(std::make_shared<StatusConvertor>());
    convertors.push_back(std::make_shared<StatusShowConvertor>());
    convertors.push_back(std::make_shared<DelayConvertor>());
    convertors.push_back(std::make_shared<CommandConvertor>(this));
    convertors.push_back(std::make_shared<PubSubEventConvertor>(this));
    convertors.push_back(std::make_shared<BodyConvertor>());
    convertors.push_back(std::make_shared<SubjectConvertor>());
    convertors.push_back(std::make_shared<VCardConvertor>());
    convertors.push_back(std::make_shared<VCardUpdateConvertor>());
    convertors.push_back(std::make_shared<FormConvertor>());
    convertors.push_back(std::make_shared<SoftwareVersionConvertor>());
    convertors.push_back(std::make_shared<DiscoInfoConvertor>());
    convertors.push_back(std::make_shared<DiscoItemsConvertor>());
    convertors.push_back(std::make_shared<IQConvertor>(this));
    convertors.push_back(std::make_shared<PresenceConvertor>(this));
    convertors.push_back(std::make_shared<MessageConvertor>(this));
    convertors.push_back(std::make_shared<ResultSetConvertor>());
    convertors.push_back(std::make_shared<ForwardedConvertor>(this));
    convertors.push_back(std::make_shared<MAMResultConvertor>(this));
    convertors.push_back(std::make_shared<MAMQueryConvertor>(this));
    convertors.push_back(std::make_shared<MAMFinConvertor>(this));
    convertors.push_back(std::make_shared<CarbonsReceivedConvertor>(this));
    convertors.push_back(std::make_shared<CarbonsSentConvertor>(this));
    convertors.push_back(std::make_shared<DOMElementConvertor>());
    convertors.push_back(std::make_shared<RawXMLElementConvertor>());
    convertors.push_back(std::make_shared<DefaultElementConvertor>());
}

LuaElementConvertors::~LuaElementConvertors() {
}

#include <Sluift/ElementConvertors/ElementConvertors.ipp>

std::shared_ptr<Element> LuaElementConvertors::convertFromLua(lua_State* L, int index) {
    if (lua_isstring(L, index)) {
        return convertFromLuaUntyped(L, index, "xml");
    }
    else if (lua_istable(L, index)) {
        lua_getfield(L, index, "_type");
        if (lua_isstring(L, -1)) {
            std::string type = lua_tostring(L, -1);
            lua_pop(L, 1);
            return convertFromLuaUntyped(L, index, type);
        }
        lua_pop(L, 1);
    }
    throw Lua::Exception("Unable to determine type");
}

std::shared_ptr<Element> LuaElementConvertors::convertFromLuaUntyped(lua_State* L, int index, const std::string& type) {
    index = Lua::absoluteOffset(L, index);
    for (auto&& convertor : convertors) {
        if (std::shared_ptr<Element> result = convertor->convertFromLua(L, index, type)) {
            return result;
        }
    }
    return std::shared_ptr<Element>();
}


int LuaElementConvertors::convertToLua(lua_State* L, std::shared_ptr<Element> payload) {
    if (boost::optional<std::string> type = doConvertToLuaUntyped(L, payload)) {
        if (lua_istable(L, -1)) {
            lua_pushstring(L, type->c_str());
            lua_setfield(L, -2, "_type");
            Lua::registerTableToString(L, -1);
        }
        else {
            assert(*type == "xml");
        }
        return 1;
    }
    return 0;
}

int LuaElementConvertors::convertToLuaUntyped(lua_State* L, std::shared_ptr<Element> payload) {
    if (doConvertToLuaUntyped(L, payload)) {
        return 1;
    }
    return 0;
}

boost::optional<std::string> LuaElementConvertors::doConvertToLuaUntyped(
        lua_State* L, std::shared_ptr<Element> payload) {
    if (!payload) {
        return LuaElementConvertor::NO_RESULT;
    }
    for (auto&& convertor : convertors) {
        if (boost::optional<std::string> type = convertor->convertToLua(L, payload)) {
            return *type;
        }
    }
    return LuaElementConvertor::NO_RESULT;
}

