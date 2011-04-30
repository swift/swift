/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Disco/ClientDiscoManager.h>

#include <Swiften/Disco/DiscoInfoResponder.h>
#include <Swiften/Disco/CapsInfoGenerator.h>
#include <Swiften/Presence/PayloadAddingPresenceSender.h>

namespace Swift {

ClientDiscoManager::ClientDiscoManager(IQRouter* iqRouter, PresenceSender* presenceSender) {
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
	capsInfo = CapsInfo::ref(new CapsInfo(CapsInfoGenerator(capsNode).generateCapsInfo(discoInfo)));
	discoInfoResponder->clearDiscoInfo();
	discoInfoResponder->setDiscoInfo(discoInfo);
	discoInfoResponder->setDiscoInfo(capsInfo->getNode() + "#" + capsInfo->getVersion(), discoInfo);
	presenceSender->setPayload(capsInfo);
}

}
