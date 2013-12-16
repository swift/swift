/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/MUC/MUCManager.h>
#include <Swiften/MUC/MUCImpl.h>

namespace Swift {

MUCManager::MUCManager(StanzaChannel* stanzaChannel, IQRouter* iqRouter, DirectedPresenceSender* presenceSender, MUCRegistry* mucRegistry) : stanzaChannel(stanzaChannel), iqRouter(iqRouter), presenceSender(presenceSender), mucRegistry(mucRegistry) {
}

MUC::ref MUCManager::createMUC(const JID& jid) {
	return boost::make_shared<MUCImpl>(stanzaChannel, iqRouter, presenceSender, jid, mucRegistry);
}

}
