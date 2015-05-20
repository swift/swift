/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamServerPortForwardingUser.h>

#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>

#include <boost/bind.hpp>

namespace Swift {

SOCKS5BytestreamServerPortForwardingUser::SOCKS5BytestreamServerPortForwardingUser(SOCKS5BytestreamServerManager* s5bServerManager) : s5bServerManager_(s5bServerManager) {
	// the server should be initialized, so we know what port to setup a forward for
	assert(s5bServerManager->isInitialized());
	if (s5bServerManager_->isPortForwardingReady()) {
		onSetup(!s5bServerManager_->getAssistedHostAddressPorts().empty());
	}
	else {
		onPortForwardingSetupConnection_ = s5bServerManager_->onPortForwardingSetup.connect(boost::bind(&SOCKS5BytestreamServerPortForwardingUser::handleServerManagerPortForwardingSetup, this, _1));
		s5bServerManager_->setupPortForwarding();
	}
}

SOCKS5BytestreamServerPortForwardingUser::~SOCKS5BytestreamServerPortForwardingUser() {
	if (s5bServerManager_->isPortForwardingReady()) {
		s5bServerManager_->removePortForwarding();
	}
}

bool SOCKS5BytestreamServerPortForwardingUser::isForwardingSetup() const {
	return s5bServerManager_->isPortForwardingReady();
}

void SOCKS5BytestreamServerPortForwardingUser::handleServerManagerPortForwardingSetup(bool successful) {
	onSetup(successful);
}

}
