/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <Swiften/Component/CoreComponent.h>

namespace Swift {
	class SoftwareVersionResponder;

	/**
	 * Provides the core functionality for writing XMPP component software.
	 *
	 * Besides connecting to an XMPP server, this class also provides interfaces for
	 * performing most component tasks on the XMPP network.
	 */
	class Component : public CoreComponent {
		public:
			Component(EventLoop* eventLoop, NetworkFactories* networkFactories, const JID& jid, const std::string& secret);
			~Component();

			/**
			 * Sets the software version of the client.
			 *
			 * This will be used to respond to version queries from other entities.
			 */
			void setSoftwareVersion(const std::string& name, const std::string& version);

		private:
			SoftwareVersionResponder* softwareVersionResponder;
	};
}
