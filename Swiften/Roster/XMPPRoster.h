/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/optional.hpp>
#include <vector>
#include <set>
#include <Swiften/Base/boost_bsignals.h>

#include <string>
#include <Swiften/JID/JID.h>
#include <Swiften/Elements/RosterItemPayload.h>
#include <Swiften/Roster/XMPPRosterItem.h>

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
			virtual std::string getNameForJID(const JID& jid) const = 0;

			/**
			 * Returns the list of groups for the given JID.
			 */
			virtual std::vector<std::string> getGroupsForJID(const JID& jid) = 0;

			/**
			 * Retrieve the items in the roster.
			 */
			virtual std::vector<XMPPRosterItem> getItems() const = 0;

			/**
			 * Retrieve the item with the given JID.
			 */
			virtual boost::optional<XMPPRosterItem> getItem(const JID&) const = 0;

			/**
			 * Retrieve the list of (existing) groups.
			 */
			virtual std::set<std::string> getGroups() const = 0;

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
			boost::signal<void (const JID&, const std::string&, const std::vector<std::string>&)> onJIDUpdated;

			/**
			 * Emitted when the roster is reset (e.g. due to logging in/logging out).
			 * After this signal is emitted, the roster is empty. It will be repopulated through
			 * onJIDAdded and onJIDRemoved events.
			 */
			boost::signal<void ()> onRosterCleared;

			/**
			 * Emitted after the last contact of the initial roster request response
			 * was added.
			 */
			boost::signal<void ()> onInitialRosterPopulated;
	};
}
