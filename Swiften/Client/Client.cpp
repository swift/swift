/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Client/Client.h>

#include <Swiften/Avatars/AvatarManagerImpl.h>
#include <Swiften/Base/SafeString.h>
#include <Swiften/Client/ClientBlockListManager.h>
#include <Swiften/Client/ClientSession.h>
#include <Swiften/Client/MemoryStorages.h>
#include <Swiften/Client/NickManagerImpl.h>
#include <Swiften/Client/NickResolver.h>
#include <Swiften/Disco/CapsManager.h>
#include <Swiften/Disco/ClientDiscoManager.h>
#include <Swiften/Disco/EntityCapsManager.h>
#include <Swiften/FileTransfer/FileTransferManagerImpl.h>
#include <Swiften/Jingle/JingleSessionManager.h>
#include <Swiften/MIX/MIXRegistry.h>
#include <Swiften/MUC/MUCManager.h>
#include <Swiften/MUC/MUCRegistry.h>
#include <Swiften/Network/NetworkFactories.h>
#include <Swiften/Presence/DirectedPresenceSender.h>
#include <Swiften/Presence/PresenceOracle.h>
#include <Swiften/Presence/StanzaChannelPresenceSender.h>
#include <Swiften/Presence/SubscriptionManager.h>
#include <Swiften/PubSub/PubSubManagerImpl.h>
#include <Swiften/Queries/Responders/SoftwareVersionResponder.h>
#include <Swiften/Roster/XMPPRosterController.h>
#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/TLS/BlindCertificateTrustChecker.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/VCards/VCardManager.h>
#include <Swiften/Whiteboard/WhiteboardSessionManager.h>

#ifndef SWIFT_EXPERIMENTAL_FT
#include <Swiften/FileTransfer/UnitTest/DummyFileTransferManager.h>
#endif

namespace Swift {

Client::Client(const JID& jid, const SafeString& password, NetworkFactories* networkFactories, Storages* storages) : CoreClient(jid, password, networkFactories), storages(storages) {
    memoryStorages = new MemoryStorages(networkFactories->getCryptoProvider());

    softwareVersionResponder = new SoftwareVersionResponder(getIQRouter());
    softwareVersionResponder->start();

    roster = new XMPPRosterImpl();
    rosterController = new XMPPRosterController(getIQRouter(), roster, getStorages()->getRosterStorage());

    subscriptionManager = new SubscriptionManager(getStanzaChannel());

    presenceOracle = new PresenceOracle(getStanzaChannel(), roster);
    presenceOracle->onPresenceChange.connect(boost::ref(onPresenceChange));

    stanzaChannelPresenceSender = new StanzaChannelPresenceSender(getStanzaChannel());
    directedPresenceSender = new DirectedPresenceSender(stanzaChannelPresenceSender);
    discoManager = new ClientDiscoManager(getIQRouter(), directedPresenceSender, networkFactories->getCryptoProvider());

    mixRegistry = std::make_unique<MIXRegistry>(getJID(), getIQRouter(), getRoster(), getStanzaChannel());

    mucRegistry = new MUCRegistry();
    mucManager = new MUCManager(getStanzaChannel(), getIQRouter(), directedPresenceSender, mucRegistry);

    vcardManager = new VCardManager(jid, getIQRouter(), getStorages()->getVCardStorage());
    avatarManager = new AvatarManagerImpl(vcardManager, getStanzaChannel(), getStorages()->getAvatarStorage(), networkFactories->getCryptoProvider(), mucRegistry);
    capsManager = new CapsManager(getStorages()->getCapsStorage(), getStanzaChannel(), getIQRouter(), networkFactories->getCryptoProvider());
    entityCapsManager = new EntityCapsManager(capsManager, getStanzaChannel());

    nickManager = new NickManagerImpl(jid.toBare(), vcardManager);
    nickResolver = new NickResolver(jid.toBare(), roster, vcardManager, mucRegistry);

    blindCertificateTrustChecker = new BlindCertificateTrustChecker();

    jingleSessionManager = new JingleSessionManager(getIQRouter());
    blockListManager = new ClientBlockListManager(getIQRouter());

    whiteboardSessionManager = nullptr;
#ifdef SWIFT_EXPERIMENTAL_WB
    whiteboardSessionManager = new WhiteboardSessionManager(getIQRouter(), getStanzaChannel(), presenceOracle, getEntityCapsProvider());
#endif

    pubsubManager = new PubSubManagerImpl(getStanzaChannel(), getIQRouter());

#ifdef SWIFT_EXPERIMENTAL_FT
    fileTransferManager = new FileTransferManagerImpl(
            getJID(),
            jingleSessionManager,
            getIQRouter(),
            getEntityCapsProvider(),
            presenceOracle,
            getNetworkFactories()->getConnectionFactory(),
            getNetworkFactories()->getConnectionServerFactory(),
            getNetworkFactories()->getTimerFactory(),
            getNetworkFactories()->getDomainNameResolver(),
            getNetworkFactories()->getNetworkEnvironment(),
            getNetworkFactories()->getNATTraverser(),
            getNetworkFactories()->getCryptoProvider());
#else
    fileTransferManager = new DummyFileTransferManager();
#endif
}

Client::~Client() {
    delete pubsubManager;
    delete whiteboardSessionManager;

    delete fileTransferManager;
    delete blockListManager;
    delete jingleSessionManager;

    delete blindCertificateTrustChecker;

    delete nickResolver;
    delete nickManager;

    delete entityCapsManager;
    delete capsManager;
    delete avatarManager;
    delete vcardManager;

    mixRegistry.reset();

    delete mucManager;
    delete mucRegistry;

    delete discoManager;
    delete directedPresenceSender;
    delete stanzaChannelPresenceSender;

    delete presenceOracle;
    delete subscriptionManager;
    delete rosterController;
    delete roster;

    softwareVersionResponder->stop();
    delete softwareVersionResponder;

    delete memoryStorages;
}

XMPPRoster* Client::getRoster() const {
    return roster;
}

void Client::setSoftwareVersion(const std::string& name, const std::string& version, const std::string& os) {
    softwareVersionResponder->setVersion(name, version, os);
}

void Client::handleConnected() {
    discoManager->handleConnected();
}

void Client::requestRoster(bool includeMIX) {
    // FIXME: We should set this once when the session is finished, but there
    // is currently no callback for this
    if (getSession()) {
        rosterController->setUseVersioning(getSession()->getRosterVersioningSupported());
    }
    rosterController->requestRoster(includeMIX);
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

PresenceSender* Client::getPresenceSender() const {
    return discoManager->getPresenceSender();
}

EntityCapsProvider* Client::getEntityCapsProvider() const {
    return entityCapsManager;
}


void Client::setAlwaysTrustCertificates() {
    setCertificateTrustChecker(blindCertificateTrustChecker);
}

NickManager* Client::getNickManager() const {
    return nickManager;
}

FileTransferManager* Client::getFileTransferManager() const {
    return fileTransferManager;
}

WhiteboardSessionManager* Client::getWhiteboardSessionManager() const {
    return whiteboardSessionManager;
}

}
