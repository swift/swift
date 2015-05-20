/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {

class SOCKS5BytestreamServerManager;

class SOCKS5BytestreamServerResourceUser {
	public:
		SOCKS5BytestreamServerResourceUser(SOCKS5BytestreamServerManager* s5bServerManager);
		~SOCKS5BytestreamServerResourceUser();

		bool isInitialized() const;

		boost::signal<void (bool /* success */)> onSuccessfulInitialized;

	private:
		void handleServerManagerInitialized(bool successfulInitialize);

	private:
		SOCKS5BytestreamServerManager* s5bServerManager_;
		boost::bsignals::scoped_connection onInitializedConnection_;
};

}
