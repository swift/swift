/*
 * Copyright (c) 2016-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/LogSerializers.h>

#include <Swiften/Elements/Presence.h>
#include <Swiften/Network/BOSHConnection.h>

namespace Swift {

std::ostream& operator<<(std::ostream& stream, const Presence& presence) {
    std::string typeString;
    switch (presence.getType()) {
        case Presence::Available:
            typeString = "Available";
            break;
        case Presence::Error:
            typeString = "Error";
            break;
        case Presence::Probe:
            typeString = "Probe";
            break;
        case Presence::Subscribe:
            typeString = "Subscribe";
            break;
        case Presence::Subscribed:
            typeString = "Subscribed";
            break;
        case Presence::Unavailable:
            typeString = "Unavailable";
            break;
        case Presence::Unsubscribe:
            typeString = "Unsubscribe";
            break;
        case Presence::Unsubscribed:
            typeString = "Unsubscribed";
            break;
    }

    std::string showTypeString;
    switch (presence.getShow()) {
        case StatusShow::Online:
            showTypeString = "Online";
            break;
        case StatusShow::Away:
            showTypeString = "Away";
            break;
        case StatusShow::FFC:
            showTypeString = "FFC";
            break;
        case StatusShow::DND:
            showTypeString = "DND";
            break;
        case StatusShow::XA:
            showTypeString = "XA";
            break;
        case StatusShow::None:
            showTypeString = "None";
            break;
    }

    stream << "Presence(" << "from: " << presence.getFrom() << ", to: " << presence.getTo() << ", type: " << typeString << ", status: " << showTypeString << ", priority: " << presence.getPriority() << ", '" << presence.getStatus() << "'" <<  " )";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const BOSHError& boshError) {
    std::string errorString;
    switch (boshError.getType()) {
        case BOSHError::BadRequest:
            errorString = "BadRequest";
            break;
        case BOSHError::HostGone:
            errorString = "HostGone";
            break;
        case BOSHError::HostUnknown:
            errorString = "HostUnknown";
            break;
        case BOSHError::ImproperAddressing:
            errorString = "ImproperAddressing";
            break;
        case BOSHError::InternalServerError:
            errorString = "InternalServerError";
            break;
        case BOSHError::ItemNotFound:
            errorString = "ItemNotFound";
            break;
        case BOSHError::OtherRequest:
            errorString = "OtherRequest";
            break;
        case BOSHError::PolicyViolation:
            errorString = "PolicyViolation";
            break;
        case BOSHError::RemoteConnectionFailed:
            errorString = "RemoteConnectionFailed";
            break;
        case BOSHError::RemoteStreamError:
            errorString = "RemoteStreamError";
            break;
        case BOSHError::SeeOtherURI:
            errorString = "SeeOtherURI";
            break;
        case BOSHError::SystemShutdown:
            errorString = "SystemShutdown";
            break;
        case BOSHError::UndefinedCondition:
            errorString = "UndefinedCondition";
            break;
        case BOSHError::NoError:
            errorString = "NoError";
            break;
    }

    stream << "BOSHError( " << errorString << " )";
    return stream;
}

};

::std::ostream& operator<<(::std::ostream& os, const boost::optional<std::string>& optStr) {
    if (optStr.is_initialized()) {
        return os << "boost::optional<std::string>(\"" << optStr.get() << "\")";
    }
    else {
        return os << "boost::optional<std::string>()";
    }
}

