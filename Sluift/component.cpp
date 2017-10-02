/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/assign/list_of.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <Swiften/Base/IDGenerator.h>
#include <Swiften/Elements/DiscoInfo.h>
#include <Swiften/Elements/MAMQuery.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Elements/RosterPayload.h>
#include <Swiften/Elements/SoftwareVersion.h>
#include <Swiften/JID/JID.h>
#include <Swiften/Presence/PresenceSender.h>
#include <Swiften/Presence/SubscriptionManager.h>
#include <Swiften/Queries/GenericRequest.h>
#include <Swiften/Queries/IQRouter.h>
#include <Swiften/Queries/Requests/GetSoftwareVersionRequest.h>
#include <Swiften/Roster/SetRosterRequest.h>
#include <Swiften/Roster/XMPPRoster.h>
#include <Swiften/Roster/XMPPRosterItem.h>

#include <Sluift/ElementConvertors/IQConvertor.h>
#include <Sluift/ElementConvertors/MessageConvertor.h>
#include <Sluift/ElementConvertors/PresenceConvertor.h>
#include <Sluift/ElementConvertors/StanzaConvertor.h>
#include <Sluift/ElementConvertors/StatusShowConvertor.h>
#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/Lua/FunctionRegistration.h>
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/Lua/Value.h>
#include <Sluift/SluiftComponent.h>
#include <Sluift/globals.h>

using namespace Swift;
namespace lambda = boost::lambda;

static inline SluiftComponent* getComponent(lua_State* L) {
    return *Lua::checkUserData<SluiftComponent>(L, 1);
}

static inline int getGlobalTimeout(lua_State* L) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.moduleLibIndex);
    lua_getfield(L, -1, "timeout");
    int result = boost::numeric_cast<int>(lua_tointeger(L, -1));
    lua_pop(L, 2);
    return result;
}

static void addPayloadsToTable(lua_State* L, const std::vector<std::shared_ptr<Payload> >& payloads) {
    if (!payloads.empty()) {
        lua_createtable(L, boost::numeric_cast<int>(payloads.size()), 0);
        for (size_t i = 0; i < payloads.size(); ++i) {
            Sluift::globals.elementConvertor.convertToLua(L, payloads[i]);
            lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
        }
        Lua::registerGetByTypeIndex(L, -1);
        lua_setfield(L, -2, "payloads");
    }
}

static std::shared_ptr<Payload> getPayload(lua_State* L, int index) {
    if (lua_type(L, index) == LUA_TTABLE) {
        return std::dynamic_pointer_cast<Payload>(Sluift::globals.elementConvertor.convertFromLua(L, index));
    }
    else if (lua_type(L, index) == LUA_TSTRING) {
        return std::make_shared<RawXMLPayload>(Lua::checkString(L, index));
    }
    else {
        return std::shared_ptr<Payload>();
    }
}

static std::vector< std::shared_ptr<Payload> > getPayloadsFromTable(lua_State* L, int index) {
    index = Lua::absoluteOffset(L, index);
    std::vector< std::shared_ptr<Payload> > result;
    lua_getfield(L, index, "payloads");
    if (lua_istable(L, -1)) {
        for (lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1)) {
            std::shared_ptr<Payload> payload = getPayload(L, -1);
            if (payload) {
                result.push_back(payload);
            }
        }
    }
    lua_pop(L, 1);
    return result;
}

SLUIFT_LUA_FUNCTION(Component, async_connect) {
    SluiftComponent* component = getComponent(L);

    std::string host;
    int port = 0;
    if (lua_istable(L, 2)) {
        if (boost::optional<std::string> hostString = Lua::getStringField(L, 2, "host")) {
            host = *hostString;
        }
        if (boost::optional<int> portInt = Lua::getIntField(L, 2, "port")) {
            port = *portInt;
        }
    }
    component->connect(host, port);
    return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, set_trace_enabled,
        "Enable/disable tracing of the data sent/received.\n\n.",
        "self\n"
        "enable  a boolean specifying whether to enable/disable tracing",
        ""
) {
    getComponent(L)->setTraceEnabled(lua_toboolean(L, 1));
    return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, wait_connected,
        "Block until the component is connected.\n\nThis is useful after an `async_connect`.",
        "self",
        ""
) {
    getComponent(L)->waitConnected(getGlobalTimeout(L));
    return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, is_connected,
        "Checks whether this component is still connected.\n\nReturns a boolean.",
        "self\n",
        ""
) {
    lua_pushboolean(L, getComponent(L)->isConnected());
    return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, disconnect,
        "Disconnect from the server",
        "self\n",
        ""
) {
    Sluift::globals.eventLoop.runOnce();
    getComponent(L)->disconnect();
    return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, set_version,

        "Sets the published version of this component.",

        "self",

        "name  the name of the component software\n"
        "version  the version identifier of this component\n"
        "os  the OS this component is running on\n"
) {
    Sluift::globals.eventLoop.runOnce();
    SluiftComponent* component = getComponent(L);
    if (std::shared_ptr<SoftwareVersion> version = std::dynamic_pointer_cast<SoftwareVersion>(Sluift::globals.elementConvertor.convertFromLuaUntyped(L, 2, "software_version"))) {
        component->setSoftwareVersion(version->getName(), version->getVersion(), version->getOS());
    }
    return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, send_message,
        "Send a message.",
        "self\n"
        "to  the JID to send the message to\n"
        "body  the body of the message. Can alternatively be specified using the `body` option\n",

        "to  the JID to send the message to\n"
        "body  the body of the message\n"
        "subject  the subject of the MUC room to set\n"
        "type  the type of message to send (`normal`, `chat`, `error`, `groupchat`, `headline`)\n"
        "payloads  payloads to add to the message\n"
) {
    Sluift::globals.eventLoop.runOnce();
    JID to;
    boost::optional<std::string> from;
    boost::optional<std::string> body;
    boost::optional<std::string> subject;
    std::vector<std::shared_ptr<Payload> > payloads;
    int index = 2;
    Message::Type type = Message::Chat;
    if (lua_isstring(L, index)) {
        to = std::string(lua_tostring(L, index));
        ++index;
        if (lua_isstring(L, index)) {
            body = lua_tostring(L, index);
            ++index;
        }
    }
    if (lua_istable(L, index)) {
        if (boost::optional<std::string> value = Lua::getStringField(L, index, "to")) {
            to = *value;
        }

        if (boost::optional<std::string> value = Lua::getStringField(L, index, "from")) {
            from = value;
        }

        if (boost::optional<std::string> value = Lua::getStringField(L, index, "body")) {
            body = value;
        }

        if (boost::optional<std::string> value = Lua::getStringField(L, index, "type")) {
            type = MessageConvertor::convertMessageTypeFromString(*value);
        }

        if (boost::optional<std::string> value = Lua::getStringField(L, index, "subject")) {
            subject = value;
        }

        payloads = getPayloadsFromTable(L, index);
    }

    if (!to.isValid()) {
        throw Lua::Exception("Missing 'to'");
    }
    if ((!body || body->empty()) && !subject && payloads.empty()) {
        throw Lua::Exception("Missing any of 'body', 'subject' or 'payloads'");
    }
    Message::ref message = std::make_shared<Message>();
    message->setTo(to);
    if (from && !from->empty()) {
        message->setFrom(*from);
    }
    if (body && !body->empty()) {
        message->setBody(*body);
    }
    if (subject) {
        message->setSubject(*subject);
    }
    message->addPayloads(payloads.begin(), payloads.end());
    message->setType(type);
    getComponent(L)->getComponent()->sendMessage(message);
    return 0;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, send_presence,
        "Send presence.",

        "self\n"
        "body  the text of the presence. Can alternatively be specified using the `status` option\n",

        "to  the JID to send the message to\n"
        "from  the JID to send the message from\n"
        "status  the text of the presence\n"
        "show  the availability of the presence (`online`, `ffc`, `away`, `xa`, `dnd`)\n"
        "priority  the priority of the presence\n"
        "type  the type of message to send (`available`, `error`, `probe`, `subscribe`, `subscribed`, `unavailable`, `unsubscribe`, `unsubscribed`)\n"
        "payloads  payloads to add to the presence\n"
) {
    Sluift::globals.eventLoop.runOnce();
    std::shared_ptr<Presence> presence = std::make_shared<Presence>();

    int index = 2;
    if (lua_isstring(L, index)) {
        presence->setStatus(lua_tostring(L, index));
        ++index;
    }
    if (lua_istable(L, index)) {
        if (boost::optional<std::string> value = Lua::getStringField(L, index, "to")) {
            presence->setTo(*value);
        }
        if (boost::optional<std::string> value = Lua::getStringField(L, index, "from")) {
            presence->setFrom(*value);
        }
        if (boost::optional<std::string> value = Lua::getStringField(L, index, "status")) {
            presence->setStatus(*value);
        }
        if (boost::optional<int> value = Lua::getIntField(L, index, "priority")) {
            presence->setPriority(*value);
        }
        if (boost::optional<std::string> value = Lua::getStringField(L, index, "type")) {
            presence->setType(PresenceConvertor::convertPresenceTypeFromString(*value));
        }
        if (boost::optional<std::string> value = Lua::getStringField(L, index, "show")) {
            presence->setShow(StatusShowConvertor::convertStatusShowTypeFromString(*value));
        }
        std::vector< std::shared_ptr<Payload> > payloads = getPayloadsFromTable(L, index);
        presence->addPayloads(payloads.begin(), payloads.end());
    }

    getComponent(L)->getComponent()->sendPresence(presence);
    lua_pushvalue(L, 1);
    return 0;
}

static int sendQuery(lua_State* L, IQ::Type type) {
    SluiftComponent* component = getComponent(L);

    JID to;
    if (boost::optional<std::string> toString = Lua::getStringField(L, 2, "to")) {
        to = JID(*toString);
    }

    JID from;
    if (boost::optional<std::string> fromString = Lua::getStringField(L, 2, "from")) {
        from = JID(*fromString);
    }

    int timeout = getGlobalTimeout(L);
    if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
        timeout = *timeoutInt;
    }

    std::shared_ptr<Payload> payload;
    lua_getfield(L, 2, "query");
    payload = getPayload(L, -1);
    lua_pop(L, 1);

    return component->sendRequest(
        std::make_shared< GenericRequest<Payload> >(type, from, to, payload, component->getComponent()->getIQRouter()), timeout).convertToLuaResult(L);
}

SLUIFT_LUA_FUNCTION(Component, get) {
    return sendQuery(L, IQ::Get);
}

SLUIFT_LUA_FUNCTION(Component, set) {
    return sendQuery(L, IQ::Set);
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, send,
        "Sends a raw string",

        "self\n"
        "data  the string to send\n",

        ""
) {
    Sluift::globals.eventLoop.runOnce();

    getComponent(L)->getComponent()->sendData(std::string(Lua::checkString(L, 2)));
    lua_pushvalue(L, 1);
    return 0;
}

static void pushEvent(lua_State* L, const SluiftComponent::Event& event) {
    switch (event.type) {
        case SluiftComponent::Event::MessageType: {
            Message::ref message = std::dynamic_pointer_cast<Message>(event.stanza);
            Lua::Table result = boost::assign::map_list_of
                ("type", std::make_shared<Lua::Value>(std::string("message")))
                ("from", std::make_shared<Lua::Value>(message->getFrom().toString()))
                ("to", std::make_shared<Lua::Value>(message->getTo().toString()))
                ("body", std::make_shared<Lua::Value>(message->getBody().get_value_or("")))
                ("message_type", std::make_shared<Lua::Value>(MessageConvertor::convertMessageTypeToString(message->getType())));
            Lua::pushValue(L, result);
            addPayloadsToTable(L, message->getPayloads());
            Lua::registerTableToString(L, -1);
            break;
        }
        case SluiftComponent::Event::PresenceType: {
            Presence::ref presence = std::dynamic_pointer_cast<Presence>(event.stanza);
            Lua::Table result = boost::assign::map_list_of
                ("type", std::make_shared<Lua::Value>(std::string("presence")))
                ("from", std::make_shared<Lua::Value>(presence->getFrom().toString()))
                ("to", std::make_shared<Lua::Value>(presence->getTo().toString()))
                ("status", std::make_shared<Lua::Value>(presence->getStatus()))
                ("presence_type", std::make_shared<Lua::Value>(PresenceConvertor::convertPresenceTypeToString(presence->getType())));
            Lua::pushValue(L, result);
            addPayloadsToTable(L, presence->getPayloads());
            Lua::registerTableToString(L, -1);
            break;
        }
    }
}

struct CallUnaryLuaPredicateOnEvent {
    CallUnaryLuaPredicateOnEvent(lua_State* L, int index) : L(L), index(index) {
    }

    bool operator()(const SluiftComponent::Event& event) {
        lua_pushvalue(L, index);
        pushEvent(L, event);
        if (lua_pcall(L, 1, 1, 0) != 0) {
            throw Lua::Exception(lua_tostring(L, -1));
        }
        bool result = lua_toboolean(L, -1);
        lua_pop(L, 1);
        return result;
    }

    lua_State* L;
    int index;
};


SLUIFT_LUA_FUNCTION(Component, get_next_event) {
    Sluift::globals.eventLoop.runOnce();
    SluiftComponent* component = getComponent(L);

    int timeout = getGlobalTimeout(L);
    boost::optional<SluiftComponent::Event::Type> type;
    int condition = 0;
    if (lua_istable(L, 2)) {
        if (boost::optional<std::string> typeString = Lua::getStringField(L, 2, "type")) {
            if (*typeString == "message") {
                type = SluiftComponent::Event::MessageType;
            }
            else if (*typeString == "presence") {
                type = SluiftComponent::Event::PresenceType;
            }
        }
        if (boost::optional<int> timeoutInt = Lua::getIntField(L, 2, "timeout")) {
            timeout = *timeoutInt;
        }
        lua_getfield(L, 2, "if");
        if (lua_isfunction(L, -1)) {
            condition = Lua::absoluteOffset(L, -1);
        }
    }

    boost::optional<SluiftComponent::Event> event;
    if (condition) {
        event = component->getNextEvent(timeout, CallUnaryLuaPredicateOnEvent(L, condition));
    }
    else if (type) {
        event = component->getNextEvent(
                timeout, lambda::bind(&SluiftComponent::Event::type, lambda::_1) == *type);
    }
    else {
        event = component->getNextEvent(timeout);
    }

    if (event) {
        pushEvent(L, *event);
    }
    else {
        lua_pushnil(L);
    }
    return 1;
}

SLUIFT_LUA_FUNCTION_WITH_HELP(
        Component, jid,
        "Returns the JID of this component",
        "self\n",
        ""
) {
    SluiftComponent* component = getComponent(L);
    lua_pushstring(L, component->getComponent()->getJID().toString().c_str());
    return 1;
}

SLUIFT_LUA_FUNCTION(Component, __gc) {
    SluiftComponent* component = getComponent(L);
    delete component;
    return 0;
}
