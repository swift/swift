/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#include <Swiften/MIX/MIX.h>

namespace Swift {

const std::string MIX::AllowedNode = std::string("urn:xmpp:mix:nodes:allowed");
const std::string MIX::BannedNode = std::string("urn:xmpp:mix:nodes:banned");
const std::string MIX::ConfigurationNode = std::string("urn:xmpp:mix:nodes:config");
const std::string MIX::InformationNode = std::string("urn:xmpp:mix:nodes:info");
const std::string MIX::JIDMapNode = std::string("urn:xmpp:mix:nodes:jidmap");
const std::string MIX::JIDMaybeVisibleMapNode = std::string("urn:xmpp:mix:nodes:jidmap2");
const std::string MIX::MessagesNode = std::string("urn:xmpp:mix:nodes:messages");
const std::string MIX::ParticipantsNode = std::string("urn:xmpp:mix:nodes:participants");
const std::string MIX::PresenceNode = std::string("urn:xmpp:mix:nodes:presence");

MIX::~MIX() {
}

}
