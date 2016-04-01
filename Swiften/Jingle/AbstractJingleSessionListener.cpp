/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Jingle/AbstractJingleSessionListener.h>

#include <Swiften/Base/Log.h>

using namespace Swift;

void AbstractJingleSessionListener::handleSessionAcceptReceived(const JingleContentID&, std::shared_ptr<JingleDescription>, std::shared_ptr<JingleTransportPayload>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleSessionInfoReceived(std::shared_ptr<JinglePayload>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleSessionTerminateReceived(boost::optional<JinglePayload::Reason>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleTransportAcceptReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleTransportInfoReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleTransportRejectReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleTransportReplaceReceived(const JingleContentID&, std::shared_ptr<JingleTransportPayload>) {
    SWIFT_LOG(warning) << "Unimplemented" << std::endl;
}

void AbstractJingleSessionListener::handleTransportInfoAcknowledged(const std::string&) {
}
