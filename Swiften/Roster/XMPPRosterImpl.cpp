/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Roster/XMPPRosterImpl.h>
#include <Swiften/Base/foreach.h>

namespace Swift {

XMPPRosterImpl::XMPPRosterImpl() {
}

void XMPPRosterImpl::addContact(const JID& jid, const std::string& name, const std::vector<std::string>& groups, RosterItemPayload::Subscription subscription) {
	JID bareJID(jid.toBare());
	std::map<JID, XMPPRosterItem>::iterator i = entries_.find(bareJID);
	if (i != entries_.end()) {
		std::string oldName = i->second.getName();
		std::vector<std::string> oldGroups = i->second.getGroups();
		i->second = XMPPRosterItem(jid, name, groups, subscription);
		onJIDUpdated(bareJID, oldName, oldGroups);
	}
	else {
		entries_.insert(std::make_pair(bareJID, XMPPRosterItem(jid, name, groups, subscription)));
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

std::string XMPPRosterImpl::getNameForJID(const JID& jid) const {
	std::map<JID, XMPPRosterItem>::const_iterator i = entries_.find(jid.toBare());
	if (i != entries_.end()) {
		return i->second.getName();
	}
	else {
		return "";
	}
}

std::vector<std::string> XMPPRosterImpl::getGroupsForJID(const JID& jid) {
	std::map<JID, XMPPRosterItem>::iterator i = entries_.find(jid.toBare());
	if (i != entries_.end()) {
		return i->second.getGroups();
	}
	else {
		return std::vector<std::string>();
	}
}

RosterItemPayload::Subscription XMPPRosterImpl::getSubscriptionStateForJID(const JID& jid) {
	std::map<JID, XMPPRosterItem>::iterator i = entries_.find(jid.toBare());
	if (i != entries_.end()) {
		return i->second.getSubscription();
	}
	else {
		return RosterItemPayload::None;
	}
}

std::vector<XMPPRosterItem> XMPPRosterImpl::getItems() const {
	std::vector<XMPPRosterItem> result;
	foreach(const RosterMap::value_type& entry, entries_) {
		result.push_back(entry.second);
	}
	return result;
}

boost::optional<XMPPRosterItem> XMPPRosterImpl::getItem(const JID& jid) const {
	std::map<JID, XMPPRosterItem>::const_iterator i = entries_.find(jid.toBare());
	if (i != entries_.end()) {
		return i->second;
	}
	else {
		return boost::optional<XMPPRosterItem>();
	}
}

std::set<std::string> XMPPRosterImpl::getGroups() const {
	std::set<std::string> result;
	foreach(const RosterMap::value_type& entry, entries_) {
		std::vector<std::string> groups = entry.second.getGroups();
		result.insert(groups.begin(), groups.end());
	}
	return result;
}

}

