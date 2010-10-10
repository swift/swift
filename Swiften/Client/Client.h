/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Client/CoreClient.h"

namespace Swift {
	class SoftwareVersionResponder;
	class XMPPRoster;
	class XMPPRosterController;

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

			/**
			 * Returns a representation of the roster.
			 *
			 * The roster is initially empty. To populate it, call requestRoster(), which
			 * will request the roster from the server. When the roster has been requested,
			 * it will also be kept up to date when it is updated on the server side.
			 *
			 * This pointer remains the same across the lifetime of Client. All changes
			 * to the roster (e.g. after the initial roster request, or after subsequent
			 * roster updates) are notified through the XMPPRoster's signals.
			 *
			 * \see requestRoster()
			 */
			XMPPRoster* getRoster() const {
				return roster;
			}

			/**
			 * Requests the roster from the server.
			 *
			 * \see getRoster()
			 */
			void requestRoster();

		private:
			SoftwareVersionResponder* softwareVersionResponder;
			XMPPRoster* roster;
			XMPPRosterController* rosterController;
	};
}
