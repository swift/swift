/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Client/Client.h"

#include "Swiften/Queries/Responders/SoftwareVersionResponder.h"
#include "Swiften/Roster/XMPPRosterImpl.h"
#include "Swiften/Roster/XMPPRosterController.h"
#include "Swiften/Presence/PresenceOracle.h"
#include "Swiften/Presence/PresenceSender.h"
#include "Swiften/MUC/MUCRegistry.h"
#include "Swiften/Client/MemoryStorages.h"
#include "Swiften/VCards/VCardManager.h"
#include "Swiften/VCards/VCardManager.h"
#include "Swiften/Avatars/AvatarManagerImpl.h"
#include "Swiften/Disco/CapsManager.h"
#include "Swiften/Disco/EntityCapsManager.h"
#include "Swiften/Client/NickResolver.h"

namespace Swift {

Client::Client(const JID& jid, const String& password, Storages* storages) : CoreClient(jid, password), storages(storages) {
	memoryStorages = new MemoryStorages();

	softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
	softwareVersionResponder->start();

	roster = new XMPPRosterImpl();
	rosterController = new XMPPRosterController(getIQRouter(), roster);

	presenceOracle = new PresenceOracle(getStanzaChannel());
	presenceOracle->onPresenceChange.connect(boost::ref(onPresenceChange));
	presenceOracle->onPresenceSubscriptionRequest.connect(boost::ref(onPresenceSubscriptionRequest));

	presenceSender = new PresenceSender(getStanzaChannel());

	mucRegistry = new MUCRegistry();

	vcardManager = new VCardManager(jid, getIQRouter(), getStorages()->getVCardStorage());
	avatarManager = new AvatarManagerImpl(vcardManager, getStanzaChannel(), getStorages()->getAvatarStorage(), mucRegistry);
	capsManager = new CapsManager(getStorages()->getCapsStorage(), getStanzaChannel(), getIQRouter());
	entityCapsManager = new EntityCapsManager(capsManager, getStanzaChannel());

	nickResolver = new NickResolver(jid.toBare(), roster, vcardManager, mucRegistry);
}

Client::~Client() {
	delete nickResolver;

	delete entityCapsManager;
	delete capsManager;
	delete avatarManager;
	delete vcardManager;

	delete mucRegistry;

	delete presenceSender;

	delete presenceOracle;
	delete rosterController;
	delete roster;

	softwareVersionResponder->stop();
	delete softwareVersionResponder;

	delete memoryStorages;
}

XMPPRoster* Client::getRoster() const {
	return roster;
}

void Client::setSoftwareVersion(const String& name, const String& version) {
	softwareVersionResponder->setVersion(name, version);
}

void Client::requestRoster() {
	rosterController->requestRoster();
}


Presence::ref Client::getLastPresence(const JID& jid) const {
	return presenceOracle->getLastPresence(jid);
}

Presence::ref Client::getHighestPriorityPresence(const JID& bareJID) const {
	return presenceOracle->getHighestPriorityPresence(bareJID);
}

Storages* Client::getStorages() const {
	if (storages) {
		return storages;
	}
	return memoryStorages;
}


}
