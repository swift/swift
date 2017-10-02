/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/MUC/MUCManager.h>

#include <Swiften/MUC/MUCImpl.h>

namespace Swift {

MUCManager::MUCManager(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, MUCRegistry* mucRegistry) : stanzaChannel(stanzaChannel), iqRouter(iqRouter), presenceSender(presenceSender), mucRegistry(mucRegistry) {
}

MUC::ref MUCManager::createMUC(const JID& jid) {
    return std::make_shared<MUCImpl>(stanzaChannel, iqRouter, presenceSender, jid, mucRegistry);
}

}
