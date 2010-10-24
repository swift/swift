/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Roster/XMPPRosterImpl.h"

namespace Swift {

XMPPRosterImpl::XMPPRosterImpl() {
}

void XMPPRosterImpl::addContact(const JID& jid, const String& name, const std::vector<String>& groups, RosterItemPayload::Subscription subscription) {
	JID bareJID(jid.toBare());
	bool exists = containsJID(bareJID);
	String oldName = getNameForJID(bareJID);
	std::vector<String> oldGroups = entries_[bareJID].groups;
	if (exists) {
		entries_.erase(bareJID);
	}
	XMPPRosterItem item;
	item.groups = groups;
	item.name = name;
	item.jid = jid;
	item.subscription = subscription;
	entries_[bareJID] = item;
	if (exists) {
		onJIDUpdated(bareJID, oldName, oldGroups);
	} else {
		onJIDAdded(bareJID);
	}
}

void XMPPRosterImpl::removeContact(const JID& jid) {
	entries_.erase(JID(jid.toBare()));
	onJIDRemoved(jid);
}

void XMPPRosterImpl::clear() {
	entries_.clear();
	onRosterCleared();
}

bool XMPPRosterImpl::containsJID(const JID& jid) {
	return entries_.find(JID(jid.toBare())) != entries_.end();
}

String XMPPRosterImpl::getNameForJID(const JID& jid) const {
	std::map<JID, XMPPRosterItem>::const_iterator i = entries_.find(jid.toBare());
	if (i != entries_.end()) {
		return i->second.name;
	}
	else {
		return "";
	}
}

const std::vector<String>& XMPPRosterImpl::getGroupsForJID(const JID& jid) {
	return entries_[JID(jid.toBare())].groups;
}

RosterItemPayload::Subscription XMPPRosterImpl::getSubscriptionStateForJID(const JID& jid) {
	return entries_[JID(jid.toBare())].subscription;
}

}

