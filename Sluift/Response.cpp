/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Response.h>

#include <lua.hpp>

#include <Swiften/Elements/ErrorPayload.h>

#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/globals.h>

using namespace Swift;
using namespace Swift::Sluift;

static std::string getErrorString(std::shared_ptr<ErrorPayload> error) {
    // Copied from ChatControllerBase.
    // TODO: Share this code;
    std::string defaultMessage = "Error sending message";
    if (!error->getText().empty()) {
        return error->getText();
    }
    else {
        switch (error->getCondition()) {
            case ErrorPayload::BadRequest: return "Bad request";
            case ErrorPayload::Conflict: return "Conflict";
            case ErrorPayload::FeatureNotImplemented: return "This feature is not implemented";
            case ErrorPayload::Forbidden: return "Forbidden";
            case ErrorPayload::Gone: return "Recipient can no longer be contacted";
            case ErrorPayload::InternalServerError: return "Internal server error";
            case ErrorPayload::ItemNotFound: return "Item not found";
            case ErrorPayload::JIDMalformed: return "JID Malformed";
            case ErrorPayload::NotAcceptable: return "Message was rejected";
            case ErrorPayload::NotAllowed: return "Not allowed";
            case ErrorPayload::NotAuthorized: return "Not authorized";
            case ErrorPayload::PaymentRequired: return "Payment is required";
            case ErrorPayload::RecipientUnavailable: return "Recipient is unavailable";
            case ErrorPayload::Redirect: return "Redirect";
            case ErrorPayload::RegistrationRequired: return "Registration required";
            case ErrorPayload::RemoteServerNotFound: return "Recipient's server not found";
            case ErrorPayload::RemoteServerTimeout: return "Remote server timeout";
            case ErrorPayload::ResourceConstraint: return "The server is low on resources";
            case ErrorPayload::ServiceUnavailable: return "The service is unavailable";
            case ErrorPayload::SubscriptionRequired: return "A subscription is required";
            case ErrorPayload::UndefinedCondition: return "Undefined condition";
            case ErrorPayload::UnexpectedRequest: return "Unexpected request";
        }
    }
    assert(false);
    return defaultMessage;
}

Response::~Response() {
}

int Response::convertToLuaResult(lua_State* L) {
    if (error) {
        lua_pushnil(L);
        lua_pushstring(L, getErrorString(error).c_str());
        bool converted = Sluift::globals.elementConvertor.convertToLuaUntyped(L, error);
        assert(converted);
        Lua::registerTableToString(L, -1);
        return 3;
    }
    else {
        if (result) {
            bool converted = Sluift::globals.elementConvertor.convertToLuaUntyped(L, result);
            assert(converted);
            Lua::registerTableToString(L, -1);
        }
        else {
            lua_pushboolean(L, 1);
        }
        return 1;
    }

}

