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
	class XMPPRosterImpl;
	class XMPPRosterController;
	class PresenceOracle;
	class PresenceSender;

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
			XMPPRoster* getRoster() const;

			/**
			 * Requests the roster from the server.
			 *
			 * \see getRoster()
			 */
			void requestRoster();

			/**
			 * Returns the last received presence for the given (full) JID.
			 */
			Presence::ref getLastPresence(const JID& jid) const;

			/**
			 * Returns the presence with the highest priority received for the given JID.
			 */
			Presence::ref getHighestPriorityPresence(const JID& bareJID) const;

			PresenceOracle* getPresenceOracle() const {
				return presenceOracle;
			}

			PresenceSender* getPresenceSender() const {
				return presenceSender;
			}

		public:
			/**
			 * This signal is emitted when a JID changes presence.
			 */
			boost::signal<void (boost::shared_ptr<Presence>)> onPresenceChange;

			/**
			 * This signal is emitted when a presence subscription request is received.
			 */
			boost::signal<void (const JID&, const String&)> onPresenceSubscriptionRequest;

		private:
			SoftwareVersionResponder* softwareVersionResponder;
			XMPPRosterImpl* roster;
			XMPPRosterController* rosterController;
			PresenceOracle* presenceOracle;
			PresenceSender* presenceSender;
	};
}
