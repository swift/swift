/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/FileTransfer/SOCKS5BytestreamServerResourceUser.h>

#include <Swiften/FileTransfer/SOCKS5BytestreamServerManager.h>

#include <boost/bind.hpp>

namespace Swift {

SOCKS5BytestreamServerResourceUser::SOCKS5BytestreamServerResourceUser(SOCKS5BytestreamServerManager* s5bServerManager) : s5bServerManager_(s5bServerManager) {
	assert(!s5bServerManager_->isInitialized());
	onInitializedConnection_ = s5bServerManager_->onInitialized.connect(boost::bind(&SOCKS5BytestreamServerResourceUser::handleServerManagerInitialized, this, _1));
	s5bServerManager_->initialize();
}

SOCKS5BytestreamServerResourceUser::~SOCKS5BytestreamServerResourceUser() {
	if (s5bServerManager_->isInitialized()) {
		s5bServerManager_->stop();
	}
}

bool SOCKS5BytestreamServerResourceUser::isInitialized() const {
	return s5bServerManager_->isInitialized();
}

void SOCKS5BytestreamServerResourceUser::handleServerManagerInitialized(bool successfulInitialize) {
	onSuccessfulInitialized(successfulInitialize);
}

}
