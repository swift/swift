/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/boost_bsignals.h>

namespace Swift {

class SOCKS5BytestreamServerManager;

class SOCKS5BytestreamServerPortForwardingUser {
	public:
		SOCKS5BytestreamServerPortForwardingUser(SOCKS5BytestreamServerManager* s5bServerManager);
		~SOCKS5BytestreamServerPortForwardingUser();

		bool isForwardingSetup() const;

		boost::signal<void (bool /* success */)> onSetup;

	private:
		void handleServerManagerPortForwardingSetup(bool successful);

	private:
		SOCKS5BytestreamServerManager* s5bServerManager_;
		boost::bsignals::scoped_connection onPortForwardingSetupConnection_;
};

}
