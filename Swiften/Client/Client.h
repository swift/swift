/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Client/CoreClient.h"

namespace Swift {
	class SoftwareVersionResponder;

	/**
	 * Provides the core functionality for writing XMPP client software.
	 *
	 * Besides connecting to an XMPP server, this class also provides interfaces for
	 * performing most tasks on the XMPP network.
	 */
	class Client : public CoreClient {
		public:
			Client(const JID& jid, const String& password);
			~Client();


			/**
			 * Sets the software version of the client.
			 *
			 * This will be used to respond to version queries from other entities.
			 */
			void setSoftwareVersion(const String& name, const String& version);

		private:
			SoftwareVersionResponder* softwareVersionResponder;

	};
}
