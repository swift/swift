/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Disco/ClientDiscoManager.h>

#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Disco/DiscoInfoResponder.h>
#include <Swiften/Presence/PayloadAddingPresenceSender.h>

namespace Swift {

ClientDiscoManager::ClientDiscoManager(IQRouter* iqRouter, PresenceSender* presenceSender, CryptoProvider* crypto) : crypto(crypto) {
    discoInfoResponder = new DiscoInfoResponder(iqRouter);
    discoInfoResponder->start();
    this->presenceSender = new PayloadAddingPresenceSender(presenceSender);
}

ClientDiscoManager::~ClientDiscoManager() {
    delete presenceSender;
    discoInfoResponder->stop();
    delete discoInfoResponder;
}

void ClientDiscoManager::setCapsNode(const std::string& node) {
    capsNode = node;
}

void ClientDiscoManager::setDiscoInfo(const DiscoInfo& discoInfo) {
    capsInfo = CapsInfo::ref(new CapsInfo(CapsInfoGenerator(capsNode, crypto).generateCapsInfo(discoInfo)));
    discoInfoResponder->clearDiscoInfo();
    discoInfoResponder->setDiscoInfo(discoInfo);
    discoInfoResponder->setDiscoInfo(capsInfo->getNode() + "#" + capsInfo->getVersion(), discoInfo);
    presenceSender->setPayload(capsInfo);
}

void ClientDiscoManager::handleConnected() {
    presenceSender->reset();
}

}
