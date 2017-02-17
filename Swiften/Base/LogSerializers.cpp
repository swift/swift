/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Base/LogSerializers.h>

#include <Swiften/Elements/Presence.h>

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

};

::std::ostream& operator<<(::std::ostream& os, const boost::optional<std::string>& optStr) {
    if (optStr.is_initialized()) {
        return os << "boost::optional<std::string>(\"" << optStr.get() << "\")";
    }
    else {
        return os << "boost::optional<std::string>()";
    }
}

