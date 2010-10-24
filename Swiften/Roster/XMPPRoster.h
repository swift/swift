/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include "Swiften/Base/String.h"
#include "Swiften/JID/JID.h"
#include "Swiften/Elements/RosterItemPayload.h"

#include <vector>
#include "Swiften/Base/boost_bsignals.h"

namespace Swift {
	/**
	 * This class represents the roster of an account, as stored on the XMPP server.
	 *
	 * Changes to the roster (either due to subscription requests or by going online/offline) are
	 * emitted through signals.
	 */
	class XMPPRoster {
		public:
			XMPPRoster();
			virtual ~XMPPRoster();

			/**
			 * Checks whether the bare jid of the given jid is in the roster.
			 */
			virtual bool containsJID(const JID& jid) = 0;

			/**
			 * Retrieves the subscription state for the given jid.
			 */
			virtual RosterItemPayload::Subscription getSubscriptionStateForJID(const JID& jid) = 0;

			/**
			 * Retrieves the stored roster name for the given jid.
			 */
			virtual String getNameForJID(const JID& jid) const = 0;

			/**
			 * Returns the list of groups for the given JID.
			 */
			virtual const std::vector<String>& getGroupsForJID(const JID& jid) = 0;

		public:
			/**
			 * Emitted when the given JID is added to the roster.
			 */
			boost::signal<void (const JID&)> onJIDAdded;

			/**
			 * Emitted when the given JID is removed from the roster.
			 */
			boost::signal<void (const JID&)> onJIDRemoved;

			/**
			 * Emitted when the name or the groups of the roster item with the
			 * given JID changes.
			 */
			boost::signal<void (const JID&, const String&, const std::vector<String>&)> onJIDUpdated;

			/**
			 * Emitted when the roster is reset (e.g. due to logging in/logging out).
			 * After this signal is emitted, the roster is empty. It will be repopulated through
			 * onJIDAdded and onJIDRemoved events.
			 */
			boost::signal<void ()> onRosterCleared;
	};
}
